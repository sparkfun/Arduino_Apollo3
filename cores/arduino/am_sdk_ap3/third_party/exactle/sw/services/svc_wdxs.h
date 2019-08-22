/*************************************************************************************************/
/*!
 *  \file   svc_wdxs.h
 *
 *  \brief  Wireless Data Exchange service implementation.
 *
 *          $Date: 2017-03-09 12:18:38 -0600 (Thu, 09 Mar 2017) $
 *          $Revision: 11460 $
 *
 *  Copyright (c) 2013-2017 ARM Ltd., all rights reserved.
 *  ARM Ltd. confidential and proprietary.
 *
 *  IMPORTANT.  Your use of this file is governed by a Software License Agreement
 *  ("Agreement") that must be accepted in order to download or otherwise receive a
 *  copy of this file.  You may not use or copy this file for any purpose other than
 *  as described in the Agreement.  If you do not agree to all of the terms of the
 *  Agreement do not use this file and delete all copies in your possession or control;
 *  if you do not have a copy of the Agreement, you must contact ARM Ltd. prior
 *  to any use, copying or further distribution of this software.
 */
/*************************************************************************************************/

#ifndef SVC_WDXS_H
#define SVC_WDXS_H

#include "bstream.h"
#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/* Base UUID:  005fXXXX-2ff2-4ed5-b045-4C7463617865 */
#define WDXS_UUID_PART1              0x65, 0x78, 0x61, 0x63, 0x74, 0x4c, 0x45, 0xb0, \
                                     0xd5, 0x4e, 0xf2, 0x2f
#define WDXS_UUID_PART2              0x5f, 0x00

/* Macro for building UUIDs */
#define WDXS_UUID_BUILD(part)        WDXS_UUID_PART1, UINT16_TO_BYTES(part), WDXS_UUID_PART2

/* WDX Service */
#define WDXS_SVC_UUID                0xFEF6

/* WDX Device Configuration Characteristic */
#define WDXS_DC_UUID                 WDXS_UUID_BUILD(0x0002)

/* WDX File Transfer Control Characteristic */
#define WDXS_FTC_UUID                WDXS_UUID_BUILD(0x0003)

/* WDX File Transfer Data Characteristic */
#define WDXS_FTD_UUID                WDXS_UUID_BUILD(0x0004)

/* WDX Authentication Characteristic */
#define WDXS_AU_UUID                 WDXS_UUID_BUILD(0x0005)

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/* Proprietary Service */
#define WDXS_START_HDL               0x240
#define WDXS_END_HDL                 (WDXS_MAX_HDL - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* Proprietary Service Handles */
enum
{
  WDXS_SVC_HDL = WDXS_START_HDL,     /* Proprietary Service Declaration */
  WDXS_DC_CH_HDL,                    /* WDX Device Configuration Characteristic Declaration */
  WDXS_DC_HDL,                       /* WDX Device Configuration Characteristic Value */
  WDXS_DC_CH_CCC_HDL,                /* WDX Device Configuration CCCD */
  WDXS_FTC_CH_HDL,                   /* WDX File Transfer Control Characteristic Declaration */
  WDXS_FTC_HDL,                      /* WDX File Transfer Control Characteristic Value */
  WDXS_FTC_CH_CCC_HDL,               /* WDX File Transfer Control CCCD */
  WDXS_FTD_CH_HDL,                   /* WDX File Transfer Data Characteristic Declaration */
  WDXS_FTD_HDL,                      /* WDX File Transfer Data Characteristic Value */
  WDXS_FTD_CH_CCC_HDL,               /* WDX File Transfer Data CCCD */
  WDXS_AU_CH_HDL,                    /* WDX Authentication Characteristic Declaration */
  WDXS_AU_HDL,                       /* WDX Authentication Characteristic Value */
  WDXS_AU_CH_CCC_HDL,                /* WDX Authentication CCCD */
  WDXS_MAX_HDL
};

/**************************************************************************************************
  Global Declarations
**************************************************************************************************/

extern const uint8_t wdxsDcUuid[ATT_128_UUID_LEN];          /* WDX Device Configuration Characteristic */
extern const uint8_t wdxsFtcUuid[ATT_128_UUID_LEN];         /* WDX File Transfer Control Characteristic */
extern const uint8_t wdxsFtdUuid[ATT_128_UUID_LEN];         /* WDX File Transfer Data Characteristic */
extern const uint8_t wdxsAuUuid[ATT_128_UUID_LEN];          /* WDX Authentication Characteristic */

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcWdxsAddGroup(void);
void SvcWdxsRemoveGroup(void);
void SvcWdxsRegister(attsWriteCback_t writeCback);

#ifdef __cplusplus
}
#endif

#endif /* SVC_WDXS_H */
