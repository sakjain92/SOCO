;***************************************************************************
;* File Name          : boot_vectors.s
;* Description        : Minimal vector table for the FOTA bootloader.
;*                      Placed at 0x08000000 via ASEGN absolute segment.
;*                      Reset handler calls Boot_ResetHandler in boot_main.c.
;*                      The application has its own full vector table at
;*                      0x08002000 (.intvec). The bootloader sets SCB->VTOR
;*                      to that address before jumping to the app.
;*
;* *** CAUTION: THIS FILE IS PART OF THE BOOTLOADER. DO NOT MODIFY  ***
;* *** WITHOUT EXTREME CARE. THE BOOTLOADER CANNOT BE UPDATED VIA   ***
;* *** FOTA — A BUG HERE REQUIRES PHYSICAL SWD ACCESS TO FIX.       ***
;***************************************************************************

        #include "boot_defs.h"

        MODULE  ?boot_cstartup

        SECTION CSTACK:DATA:NOROOT(3)

        EXTERN  Boot_ResetHandler

        ; Place vector table at absolute address BOOT_FLASH_START.
        ; The bootloader does not enable any NVIC IRQs, so peripheral
        ; interrupts cannot fire while VTOR = 0x08000000. We populate
        ; only the 16 system-exception slots and trap any unexpected
        ; fault into Boot_FaultTrap, which spins until the IWDG resets
        ; the chip. Without this, a stray HardFault/NMI would fetch a
        ; "handler" from inside boot_main.o code → garbage execution.
        ;
        ASEGN `.boot_intvec`:CODE:NOROOT(2),BOOT_FLASH_START
        DATA
        DCD     sfe(CSTACK)               ; 0x00 Initial SP
        DCD     Boot_Reset_Handler        ; 0x04 Reset
        DCD     Boot_FaultTrap            ; 0x08 NMI
        DCD     Boot_FaultTrap            ; 0x0C HardFault
        DCD     Boot_FaultTrap            ; 0x10 MemManage
        DCD     Boot_FaultTrap            ; 0x14 BusFault
        DCD     Boot_FaultTrap            ; 0x18 UsageFault
        DCD     0                         ; 0x1C Reserved
        DCD     0                         ; 0x20 Reserved
        DCD     0                         ; 0x24 Reserved
        DCD     0                         ; 0x28 Reserved
        DCD     Boot_FaultTrap            ; 0x2C SVCall
        DCD     0                         ; 0x30 Reserved (Debug Monitor)
        DCD     0                         ; 0x34 Reserved
        DCD     Boot_FaultTrap            ; 0x38 PendSV
        DCD     Boot_FaultTrap            ; 0x3C SysTick

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Reset handler — jumps to Boot_ResetHandler() in boot_main.c.
;; Fallback infinite loop in case it ever returns.
;;
        THUMB
        SECTION BOOT_SECTION:CODE:REORDER:NOROOT(2)
Boot_Reset_Handler
        LDR     R0, =Boot_ResetHandler
        BLX     R0
        B       Boot_Reset_Handler

;;
;; Fault trap — any unexpected exception during bootloader execution
;; lands here. Spin until the IWDG resets the chip. The IWDG is started
;; in Boot_ResetHandler before any user logic runs, so recovery is
;; bounded by the IWDG timeout (~2s with PR=2, RLR=0xFFF, LSI ~40kHz).
;;
Boot_FaultTrap
        B       Boot_FaultTrap

        END
