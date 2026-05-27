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
        ; Only SP and Reset_Handler are needed — interrupts are disabled
        ; during boot, so no other vectors are required.
        ;
        ASEGN `.boot_intvec`:CODE:NOROOT(2),BOOT_FLASH_START
        DATA
        DCD     sfe(CSTACK)               ; Initial SP
        DCD     Boot_Reset_Handler        ; Reset Handler

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

        END
