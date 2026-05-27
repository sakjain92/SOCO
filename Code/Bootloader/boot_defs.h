/*
******************************************************************************
  * @file    boot_defs.h
  * @brief   Pure preprocessor constants shared between bootloader
  *          assembly (.s), bootloader C, and application C.
  *
  *          This file contains ONLY preprocessor macros — no types,
  *          no structs, no C-only includes. Safe for both C and IAR
  *          assembly.
  *
  * *** CAUTION: DO NOT MODIFY THIS FILE WITHOUT UPDATING BOTH THE   ***
  * *** BOOTLOADER AND THE APPLICATION. THE BOOTLOADER LIVES IN A    ***
  * *** PROTECTED FLASH REGION THAT IS NOT UPDATED BY FOTA. ANY      ***
  * *** MISMATCH BETWEEN THIS HEADER AND THE BOOTLOADER BINARY WILL  ***
  * *** BRICK THE DEVICE.                                            ***
******************************************************************************
*/

#ifndef BOOT_DEFS_H
#define BOOT_DEFS_H

/* Flash layout */

#define BOOT_FLASH_START        0x08000000
#define BOOT_FLASH_SIZE         0x2000      /* 8 KB bootloader region (pages 0-3).
                                             * NOTE: The current bootloader uses
                                             * ~1.7 KB of this. It can be shrunk
                                             * to 0x1000 (4 KB, pages 0-1) to
                                             * reclaim 4 KB for the application
                                             * region. To do so, also update:
                                             *   - BOOT_APP_ADDRESS  (0x08001000)
                                             *   - APP_ROM start in the ICF
                                             *   - BOOTLOADER_SIZE in fota_pack.py
                                             * Bootloader cannot be updated by
                                             * FOTA, so this is a one-time SWD
                                             * reflash operation. */
#define BOOT_APP_ADDRESS        0x08002000
#define BOOT_FOTA_FLASH_ADDR    0x0801F000
#define BOOT_CALIB_ADDR         0x0801F800
#define BOOT_APP_END            BOOT_FOTA_FLASH_ADDR
#define BOOT_SRAM_TOP           0x20009000

/* EEPROM layout */

#define EEPROM_PAGE_LENGTH      64
#define EXT_EEPROM              0xA0
#define EEPROM_SIZE             0x20000     /* 128KB = 1Mbit */
#define FOTA_STAGING_START      0x8000
#define FOTA_MAX_FW_SIZE        (EEPROM_SIZE - FOTA_STAGING_START)
                                            /* 96 KB max firmware payload, set by
                                             * EEPROM staging capacity (the upper
                                             * 96 KB of the 128 KB EEPROM).
                                             *
                                             * If a future firmware exceeds 96 KB
                                             * we can extend this by reclaiming
                                             * free EEPROM at the lower end
                                             * (0x0000-0x7FFF currently holds
                                             * runtime config + logs — audit
                                             * usage and lower FOTA_STAGING_START
                                             * to grow the staging window).
                                             *
                                             * The flash app region itself is
                                             * larger (0x1D000 = 116 KB from
                                             * 0x08002000 to 0x0801EFFF), so
                                             * flash is not the bottleneck. */

/* XOR PRNG encryption (16-bit LCG, full period 2^16) */

#define FOTA_SECRET_KEY         0x4A7B
#define FOTA_SEED_MIXER         0x9E37
#define FOTA_PRNG_MUL           25173
#define FOTA_PRNG_INC           13849

/* File header: [totalRecords:2][version:2][crc:2] little-endian */

#define FOTA_HEADER_SIZE        6

#endif /* BOOT_DEFS_H */
