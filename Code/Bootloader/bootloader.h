/*
******************************************************************************
  * @file    bootloader.h
  * @brief   Shared C definitions between FOTA bootloader and application.
  *
  *          Includes boot_defs.h (pure macros, assembly-safe) and adds
  *          the FotaFlashInfo struct. Both boot_main.c and the
  *          application's Struct.h include this file.
  *
  * *** CAUTION: DO NOT MODIFY THIS FILE WITHOUT UPDATING BOTH THE   ***
  * *** BOOTLOADER AND THE APPLICATION. THE BOOTLOADER LIVES IN A    ***
  * *** PROTECTED FLASH REGION THAT IS NOT UPDATED BY FOTA. ANY      ***
  * *** MISMATCH BETWEEN THIS HEADER AND THE BOOTLOADER BINARY WILL  ***
  * *** BRICK THE DEVICE.                                            ***
******************************************************************************
*/

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "boot_defs.h"
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* FotaFlashInfo — written to flash page 62 by the application when    */
/* triggering an upgrade. Read by the bootloader on reset.             */
/*                                                                     */
/* WARNING: Do not reorder or resize existing fields. The bootloader   */
/* reads this struct directly from flash. Use the reserved[] area for  */
/* new fields. upgradePending MUST remain the last byte.               */
/* ------------------------------------------------------------------ */

#pragma pack(push, 1)
struct FotaFlashInfo
{
    uint32_t firmwareSize;    /* offset 0,  4 bytes — encrypted FW in EEPROM */
    uint16_t prngSeed;        /* offset 4,  2 bytes — derived decryption seed */
    uint16_t expectedCrc;     /* offset 6,  2 bytes — CRC-16 of plaintext FW */
    uint8_t  reserved[119];   /* offset 8,  119 bytes — for future use */
    uint8_t  upgradePending;  /* offset 127, 1 byte — 0x01 = pending */
};
#pragma pack(pop)

#endif /* BOOTLOADER_H */
