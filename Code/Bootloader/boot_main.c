/*
******************************************************************************
  * @file    boot_main.c
  * @brief   FOTA bootloader for SOCO.
  *
  *          Lives in the first 8KB of flash (pages 0-3). On reset, checks
  *          flash page 62 for an upgrade-pending flag. If set, reads
  *          encrypted firmware from external EEPROM, decrypts it, programs
  *          it into the application flash region, and resets. If not set,
  *          jumps to the application at 0x08002000.
  *
  *          All functions in this file are self-contained — they do not
  *          call any code in the application region, which gets erased
  *          during an upgrade. Place this file in the BOOT_SECTION via
  *          the linker.
  *
  * *** CAUTION: THIS FILE IS PART OF THE BOOTLOADER. DO NOT MODIFY  ***
  * *** WITHOUT EXTREME CARE. THE BOOTLOADER CANNOT BE UPDATED VIA   ***
  * *** FOTA — A BUG HERE REQUIRES PHYSICAL SWD ACCESS TO FIX.       ***
******************************************************************************
*/

#include "bootloader.h"
#include "stm32f37x.h"
#include <string.h>

#define BOOT_FLASH_KEY1         0x45670123
#define BOOT_FLASH_KEY2         0xCDEF89AB
#define BOOT_FLASH_TIMEOUT      0x000B0000

#define BOOT_IWDG_FEED()        (IWDG->KR = 0xAAAA)

/* ------------------------------------------------------------------ */
/* LED pins — active low (BRR = on, BSRR = off)                      */
/* ------------------------------------------------------------------ */

/*
Inf: Turn on all front-panel LEDs to signal bootloader activity.
Inp: None
Ret: None
*/
static void Boot_LedsAllOn(void)
{
    GPIOF->BRR  = (1u << 10);  /* LED1 */
    GPIOC->BRR  = (1u << 0);   /* LED2 */
    GPIOE->BRR  = (1u << 6);   /* LED3 */
    GPIOE->BRR  = (1u << 5);   /* LED4 */
    GPIOE->BRR  = (1u << 4);   /* LED5 */
    GPIOE->BRR  = (1u << 3);   /* LED6 */
    GPIOE->BRR  = (1u << 2);   /* LED7 */
    GPIOE->BRR  = (1u << 1);   /* LED8 */
    GPIOC->BRR  = (1u << 1);   /* LED9 */
}

/* ------------------------------------------------------------------ */
/* Clock                                                               */
/* ------------------------------------------------------------------ */

/*
Inf: Configure system clock — HSE + PLL x3 = 48MHz, HCLK /4 = 12MHz.
     Identical to the application's SetSysClock().
Inp: None
Ret: None
*/
static void Boot_SetSysClock(void)
{
    __IO uint32_t counter = 0;

    RCC->CR |= RCC_CR_HSEON;
    do { counter++; }
    while (!(RCC->CR & RCC_CR_HSERDY) && counter < 0x5000);

    if (!(RCC->CR & RCC_CR_HSERDY))
        return;

    FLASH->ACR = 0x00;
    RCC->CFGR |= RCC_CFGR_HPRE_DIV4;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;

    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1
                 | RCC_CFGR_PLLMULL3;

    RCC->CR |= RCC_CR_PLLON;
    counter = 0;
    while (!(RCC->CR & RCC_CR_PLLRDY) && ++counter < 0x5000) {}
    if (!(RCC->CR & RCC_CR_PLLRDY))
        return;

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    counter = 0;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL && ++counter < 0x5000) {}
}

/* ------------------------------------------------------------------ */
/* GPIO and peripheral clocks                                          */
/* ------------------------------------------------------------------ */

/*
Inf: Enable GPIO and I2C2 clocks, configure I2C2 pins (PA9/PA10).
     Only the peripherals needed by the bootloader are touched.
Inp: None
Ret: None
*/
static void Boot_InitGpio(void)
{
    /* Enable GPIO A, C, E, F and I2C2 clocks */
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN
                   | RCC_AHBENR_GPIOEEN | RCC_AHBENR_GPIOFEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN | RCC_APB1ENR_PWREN;

    /* PA9  = I2C2_SCL: alternate, open-drain, pull-up, AF4 */
    /* PA10 = I2C2_SDA: alternate, open-drain, pull-up, AF4 */
    GPIOA->MODER   &= ~((3u << 18) | (3u << 20));
    GPIOA->MODER   |=  ((2u << 18) | (2u << 20));   /* Alternate */
    GPIOA->OTYPER  |=  ((1u <<  9) | (1u << 10));   /* Open drain */
    GPIOA->PUPDR   &= ~((3u << 18) | (3u << 20));
    GPIOA->PUPDR   |=  ((1u << 18) | (1u << 20));   /* Pull-up */
    GPIOA->AFR[1]  &= ~((0xFu << 4) | (0xFu << 8));
    GPIOA->AFR[1]  |=  ((4u   << 4) | (4u   << 8)); /* AF4 = I2C2 */

    /* LED pins: output push-pull (PE1-PE6, PC0, PC1, PF10) */
    GPIOE->MODER |= (1u << 2) | (1u << 4) | (1u << 6)
                   | (1u << 8) | (1u << 10) | (1u << 12);
    GPIOC->MODER |= (1u << 0) | (1u << 2);
    GPIOF->MODER |= (1u << 20);
}

/* ------------------------------------------------------------------ */
/* I2C2                                                                */
/* ------------------------------------------------------------------ */

/*
Inf: Initialize I2C2 peripheral for 100kHz operation.
Inp: None
Ret: None
*/
static void Boot_InitI2C(void)
{
    /* Select SYSCLK (48MHz) as I2C2 clock — TIMINGR is computed for 48MHz */
    RCC->CFGR3 |= (1u << 5);   /* I2C2SW = 1 */

    I2C2->CR1    = 0;
    I2C2->CR2    = 0;
    I2C2->OAR1   = 0;
    I2C2->OAR2   = 0;
    I2C2->TIMINGR = 0x10805E89;  /* 48MHz source, 100kHz, analog filter on */
    I2C2->CR1   |= I2C_CR1_PE;
}

/* ------------------------------------------------------------------ */
/* Watchdog                                                            */
/* ------------------------------------------------------------------ */

/*
Inf: Start the independent watchdog with prescaler /16 and max reload.
Inp: None
Ret: None
*/
static void Boot_InitWatchdog(void)
{
    IWDG->KR  = 0x5555;
    IWDG->PR  = 0x02;     /* /16 */
    IWDG->KR  = 0x5555;
    IWDG->RLR = 0xFFF;
    IWDG->KR  = 0xCCCC;   /* Start */
}

/* ------------------------------------------------------------------ */
/* EEPROM read (self-contained, no dependency on I2CDriver.c)          */
/* ------------------------------------------------------------------ */

#define BOOT_I2C_TIMEOUT  10000

/*
Inf: Low-level I2C read from external EEPROM. Supports block-select
     addressing for EEPROMs >256Kbit.
Inp: addr       - EEPROM byte address (up to 17 bits)
     len        - number of bytes to read (max 64)
     deviceAddr - I2C slave base address (0xA0)
     buf        - output buffer
Ret: None
*/
static void Boot_I2CRead(uint32_t addr, uint8_t len,
                          uint8_t deviceAddr, uint8_t *buf)
{
    uint32_t cnt;
    uint8_t slave = deviceAddr | (((addr >> 16) & 0x3) << 1);

    I2C2->CR1 |= I2C_CR1_PE;

    /* Send 2-byte address */
    I2C2->CR2 = slave;
    I2C2->CR2 &= ~I2C_CR2_RD_WRN;
    I2C2->CR2 |= I2C_CR2_START | (2u << 16);
    cnt = 0; while ((I2C2->CR2 & I2C_CR2_START) && ++cnt < BOOT_I2C_TIMEOUT);

    I2C2->TXDR = (addr >> 8) & 0xFF;
    cnt = 0; while (!(I2C2->ISR & I2C_ISR_TXE) && ++cnt < BOOT_I2C_TIMEOUT);

    I2C2->TXDR = addr & 0xFF;
    cnt = 0; while (!(I2C2->ISR & I2C_ISR_TXE) && ++cnt < BOOT_I2C_TIMEOUT);

    /* Restart for read */
    I2C2->CR2 |= I2C_CR2_RD_WRN;
    I2C2->CR2 = (I2C2->CR2 & ~0xFF0000u) | ((uint32_t)len << 16);
    I2C2->CR2 |= I2C_CR2_START;
    cnt = 0; while ((I2C2->CR2 & I2C_CR2_START) && ++cnt < BOOT_I2C_TIMEOUT);

    for (uint8_t i = 0; i < len; i++)
    {
        cnt = 0;
        while (!(I2C2->ISR & I2C_ISR_RXNE) && ++cnt < BOOT_I2C_TIMEOUT);
        buf[i] = I2C2->RXDR;
    }

    I2C2->CR2 |= I2C_CR2_STOP;
    cnt = 0; while (!(I2C2->ISR & I2C_ISR_STOPF) && ++cnt < BOOT_I2C_TIMEOUT);
    I2C2->ICR = I2C_ICR_STOPCF;
    cnt = 0; while ((I2C2->ISR & I2C_ISR_BUSY) && ++cnt < BOOT_I2C_TIMEOUT);

    I2C2->CR1 &= ~I2C_CR1_PE;
}

/*
Inf: Read a contiguous region from EEPROM, split into page-sized chunks.
     Feeds the watchdog between pages.
Inp: addr       - EEPROM start address
     len        - total bytes to read
     deviceAddr - I2C slave base address
     buf        - output buffer
Ret: None
*/
static void Boot_EepromRead(uint32_t addr, uint32_t len,
                             uint8_t deviceAddr, uint8_t *buf)
{
    while (len)
    {
        uint8_t chunk = (len > EEPROM_PAGE_LENGTH)
                        ? EEPROM_PAGE_LENGTH : (uint8_t)len;
        Boot_I2CRead(addr, chunk, deviceAddr, buf);
        BOOT_IWDG_FEED();
        addr += chunk;
        buf  += chunk;
        len  -= chunk;
    }
}

/* ------------------------------------------------------------------ */
/* Flash operations (self-contained, no dependency on stm32f37x_flash) */
/* ------------------------------------------------------------------ */

/*
Inf: Wait until the flash controller is not busy.
Inp: None
Ret: 0 on success, non-zero on timeout
*/
static uint32_t Boot_FlashWait(void)
{
    uint32_t timeout = BOOT_FLASH_TIMEOUT;
    while ((FLASH->SR & FLASH_SR_BSY) && --timeout);
    return (timeout == 0) ? 1 : 0;
}

/*
Inf: Unlock the flash controller for erase/program operations.
Inp: None
Ret: None
*/
static void Boot_FlashUnlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = BOOT_FLASH_KEY1;
        FLASH->KEYR = BOOT_FLASH_KEY2;
    }
}

/*
Inf: Lock the flash controller.
Inp: None
Ret: None
*/
static void Boot_FlashLock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

/*
Inf: Erase a single flash page (2KB on STM32F373).
Inp: pageAddr - start address of the page
Ret: 0 on success, non-zero on error
*/
static uint32_t Boot_FlashErasePage(uint32_t pageAddr)
{
    if (Boot_FlashWait()) return 1;
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR  = pageAddr;
    FLASH->CR |= FLASH_CR_STRT;
    uint32_t r = Boot_FlashWait();
    FLASH->CR &= ~FLASH_CR_PER;
    return r;
}

/*
Inf: Program a half-word (16 bits) to flash.
Inp: addr - flash address (must be half-word aligned)
     data - 16-bit value to program
Ret: 0 on success, non-zero on error
*/
static uint32_t Boot_FlashProgramHalfWord(uint32_t addr, uint16_t data)
{
    if (Boot_FlashWait()) return 1;
    FLASH->CR |= FLASH_CR_PG;
    *(__IO uint16_t *)addr = data;
    uint32_t r = Boot_FlashWait();
    FLASH->CR &= ~FLASH_CR_PG;
    return r;
}

/* ------------------------------------------------------------------ */
/* CRC-16 — matches CRCCalculation() in DataManagment.c               */
/* ------------------------------------------------------------------ */

/*
Inf: CRC-16 over uint16_t words with caller-supplied seed.
Inp: data   - pointer to uint16_t array
     nWords - number of words
     seed   - initial CRC value
Ret: Updated CRC-16
*/
static uint16_t Boot_Crc16(const uint16_t *data, uint32_t nWords, uint16_t seed)
{
    uint16_t crc = seed;
    for (uint32_t i = 0; i < nWords; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/* ------------------------------------------------------------------ */
/* Jump to application                                                 */
/* ------------------------------------------------------------------ */

/*
Inf: Relocate vector table and jump to application at BOOT_APP_ADDRESS.
     Does not return.
Inp: None
Ret: None (does not return)
*/
static void Boot_JumpToApp(void)
{
    uint32_t sp = *(volatile uint32_t *)(BOOT_APP_ADDRESS);
    uint32_t pc = *(volatile uint32_t *)(BOOT_APP_ADDRESS + 4);

    /* Validate stack pointer is in SRAM */
    if (sp < 0x20000000 || sp > BOOT_SRAM_TOP)
    {
        /* Invalid app — hang and let watchdog reset */
        while (1) {}
    }

    __enable_irq();
    SCB->VTOR = BOOT_APP_ADDRESS;
    __set_MSP(sp);
    ((void (*)(void))pc)();

    /* Should never reach here */
    while (1) {}
}

/* ------------------------------------------------------------------ */
/* Main upgrade logic                                                  */
/* ------------------------------------------------------------------ */

/*
Inf: Read FotaFlashInfo from page 62, validate, and if upgrade is
     pending: read encrypted firmware from EEPROM, decrypt with
     16-bit LCG PRNG, program to application flash, verify, clear
     the upgrade flag, and reset.
Inp: None
Ret: None (resets on success, returns on no-upgrade or failure)
*/
static void Boot_CheckAndUpgrade(void)
{
    /* Read FotaFlashInfo from flash page 62 */
    const volatile struct FotaFlashInfo *info =
        (const volatile struct FotaFlashInfo *)BOOT_FOTA_FLASH_ADDR;
    uint32_t fwSize      = info->firmwareSize;
    uint16_t seed        = info->prngSeed;
    uint16_t expectedCrc = info->expectedCrc;
    uint8_t  pending     = info->upgradePending;

    if (pending != 0x01)
        return;

    if (fwSize == 0 || fwSize > FOTA_MAX_FW_SIZE ||
        fwSize > (BOOT_APP_END - BOOT_APP_ADDRESS))
        return;

    Boot_LedsAllOn();

    /* --- Pass 1: Read EEPROM, decrypt, validate CRC --- */
    /* Done BEFORE erasing app flash so that a corrupt or dead EEPROM */
    /* does not brick the device.                                     */
    {
        uint16_t prng      = seed;
        uint16_t crc       = 0xFFFF;
        uint32_t remaining = fwSize;
        uint32_t eepAddr   = FOTA_STAGING_START;
        uint8_t  buf[EEPROM_PAGE_LENGTH];

        while (remaining > 0)
        {
            BOOT_IWDG_FEED();
            uint32_t chunk = (remaining > EEPROM_PAGE_LENGTH)
                             ? EEPROM_PAGE_LENGTH : remaining;
            Boot_EepromRead(eepAddr, chunk, EXT_EEPROM, buf);

            for (uint32_t i = 0; i < chunk; i += 2)
            {
                prng = prng * FOTA_PRNG_MUL + FOTA_PRNG_INC;
                uint16_t enc = (uint16_t)buf[i] | ((uint16_t)buf[i + 1] << 8);
                uint16_t dec = enc ^ prng;
                crc = Boot_Crc16(&dec, 1, crc);
            }

            eepAddr   += chunk;
            remaining -= chunk;
        }

        if (crc != expectedCrc)
        {
            /* EEPROM data is corrupt or unreadable. Clear upgrade   */
            /* flag to prevent retry loops and boot existing app.    */
            Boot_FlashUnlock();
            Boot_FlashErasePage(BOOT_FOTA_FLASH_ADDR);
            Boot_FlashLock();
            return;
        }
    }

    /* --- Erase application pages (pages 4-61) --- */
    Boot_FlashUnlock();
    for (uint32_t page = BOOT_APP_ADDRESS; page < BOOT_APP_END; page += 0x800)
    {
        BOOT_IWDG_FEED();
        Boot_FlashErasePage(page);
    }

    /* --- Pass 2: Read EEPROM again, decrypt, program to flash --- */
    {
        uint16_t prng      = seed;
        uint32_t remaining = fwSize;
        uint32_t flashAddr = BOOT_APP_ADDRESS;
        uint32_t eepAddr   = FOTA_STAGING_START;
        uint8_t  buf[EEPROM_PAGE_LENGTH];

        while (remaining > 0)
        {
            BOOT_IWDG_FEED();
            uint32_t chunk = (remaining > EEPROM_PAGE_LENGTH)
                             ? EEPROM_PAGE_LENGTH : remaining;
            Boot_EepromRead(eepAddr, chunk, EXT_EEPROM, buf);

            for (uint32_t i = 0; i < chunk; i += 2)
            {
                prng = prng * FOTA_PRNG_MUL + FOTA_PRNG_INC;
                uint16_t enc = (uint16_t)buf[i] | ((uint16_t)buf[i + 1] << 8);
                uint16_t dec = enc ^ prng;
                Boot_FlashProgramHalfWord(flashAddr, dec);
                flashAddr += 2;
            }

            eepAddr   += chunk;
            remaining -= chunk;
        }
    }

    Boot_FlashLock();

    /* --- Verify: read back flash and compare CRC --- */
    uint16_t verifyCrc = Boot_Crc16((const uint16_t *)BOOT_APP_ADDRESS,
                                     fwSize / 2, 0xFFFF);
    if (verifyCrc != expectedCrc)
    {
        /* Flash write failed. Leave upgrade flag set so we retry
           on next reset. Watchdog will reset us. */
        while (1) {}
    }

    /* --- Clear upgrade flag: erase page 62 --- */
    Boot_FlashUnlock();
    Boot_FlashErasePage(BOOT_FOTA_FLASH_ADDR);
    Boot_FlashLock();

    NVIC_SystemReset();
}

/* ------------------------------------------------------------------ */
/* Boot entry point                                                    */
/* ------------------------------------------------------------------ */

/*
Inf: Bootloader entry point. Called from the boot vector table's
     Reset_Handler. Initializes minimal hardware, checks for pending
     FOTA upgrade, and either performs the upgrade or jumps to the
     application.
Inp: None
Ret: None (never returns)
*/
void Boot_ResetHandler(void)
{
    /* Minimal system init */
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
    #endif

    RCC->CR |= 0x00000001;              /* HSI on */
    RCC->CFGR &= 0x00FF0000;
    RCC->CR &= 0xFEF6FFFF;              /* HSE, CSS, PLL off */
    RCC->CR &= 0xFFFBFFFF;              /* HSEBYP off */
    RCC->CFGR &= 0xFF80FFFF;
    RCC->CFGR2 &= 0xFFFFFFF0;
    RCC->CFGR3 &= 0xFFF0F8C;
    RCC->CIR = 0;

    Boot_SetSysClock();
    SCB->VTOR = 0x08000000;

    Boot_InitGpio();
    Boot_InitI2C();
    Boot_InitWatchdog();

    __disable_irq();

    Boot_CheckAndUpgrade();

    /* No upgrade — jump to application */
    Boot_JumpToApp();
}
