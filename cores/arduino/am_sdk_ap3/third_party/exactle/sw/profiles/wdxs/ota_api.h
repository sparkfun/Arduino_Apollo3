/*************************************************************************************************/
/*!
 *  \file   ota_api.h
 *
 *  \brief  OTA flash storage utilities.
 *
 *          $Date: 2014-03-26 07:50:54 -0700 (Wed, 26 Mar 2014) $
 *          $Revision: 1251 $
 *
 *  Copyright (c) 2013 Wicentric, Inc., all rights reserved.
 *  Wicentric confidential and proprietary.
 *
 *  IMPORTANT.  Your use of this file is governed by a Software License Agreement
 *  ("Agreement") that must be accepted in order to download or otherwise receive a
 *  copy of this file.  You may not use or copy this file for any purpose other than
 *  as described in the Agreement.  If you do not agree to all of the terms of the
 *  Agreement do not use this file and delete all copies in your possession or control;
 *  if you do not have a copy of the Agreement, you must contact Wicentric, Inc. prior
 *  to any use, copying or further distribution of this software.
 */
/*************************************************************************************************/
#ifndef OTA_API_H
#define OTA_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wsf_types.h"

/**************************************************************************************************
  Config
**************************************************************************************************/

/* ROM memory partition definition 
 *
 *      ---------------- 0x40000 (256KB)
 *      | ROM_SEG_CFG  |
 *      ---------------- 0x3F000
 *      | ROM_SEG_DATA |
 *      ---------------- 0x20000
 *      | ROM_SEG_RUN  |
 *      ---------------- 0x01000
 *      | ROM_SEG_BOOT |
 *      ---------------- 0x00000
 *
 * Note: ROM_SEG_RUN value must match the starting address in the application's linker 
 * definition file. 
 *
 * Note: Segements must start on multiples of FLASH_PAGE_SIZE. */
#define ROM_SEG_BOOT        0x00000
#define ROM_SEG_BOOT_SIZE   (4 * 1024)
#define ROM_SEG_RUN         (ROM_SEG_BOOT + ROM_SEG_BOOT_SIZE)
#define ROM_SEG_RUN_SIZE    (124 * 1024)
#define ROM_SEG_DATA        (ROM_SEG_RUN + ROM_SEG_RUN_SIZE)
#define ROM_SEG_DATA_SIZE   (124 * 1024)
#define ROM_SEG_CFG         (ROM_SEG_DATA + ROM_SEG_DATA_SIZE)
#define ROM_SEG_CFG_SIZE    (4 * 1024)

/*! Flash page size in bytes. */
#define FLASH_PAGE_SIZE     2048

/* Flash image offset (ARM requirements for vectors tables to be aligned to 256 bytes). */
#define FLASH_FILE_OFFSET   0x100

/**************************************************************************************************
  Functions
**************************************************************************************************/

/* Initialization. */
void OtaInit(void);
void OtaDeInit(void);

/* Bootloader operations. */
bool_t OtaNewFileAvailable(void);
bool_t OtaCheckExistingFileCrc(void);
bool_t OtaCopyDownImage(void);
bool_t OtaCopyUpImage(void);

/* OTA profile operations. */
bool_t OtaEraseNewFileBank(void);
bool_t OtaWriteNewFile(uint32_t offset, uint16_t blkLen, const uint8_t *pBlk);
void OtaSetNewFileInfo(uint32_t len, uint32_t crc);
bool_t OtaCheckNewFileCrc(void);

#ifdef __cplusplus
};
#endif

#endif /* OTA_API_H */
