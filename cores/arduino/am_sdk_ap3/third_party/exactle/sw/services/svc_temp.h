/*************************************************************************************************/
/*!
 *  \file   svc_temp.h
 *
 *  \brief  Example temperature sensor service implementation.
*
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2015-2017 ARM Ltd., all rights reserved.
 *  ARM confidential and proprietary.
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

#ifndef SVC_TEMP_H
#define SVC_TEMP_H

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

#define TEMP_HANDLE_START  0x60
#define TEMP_HANDLE_END   (TEMP_HANDLE_END_PLUS_ONE - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* Temperature service handles. */
enum
{
  TEMP_HANDLE_SVC = TEMP_HANDLE_START,

  TEMP_HANDLE_DATA_CHR,
  TEMP_HANDLE_DATA,
  TEMP_HANDLE_DATA_CLIENT_CHR_CONFIG,
  TEMP_HANDLE_DATA_CHR_USR_DESCR,

  TEMP_HANDLE_CONFIG_CHR,
  TEMP_HANDLE_CONFIG,
  TEMP_HANDLE_CONFIG_CHR_USR_DESCR,

  TEMP_HANDLE_PERIOD_CHR,
  TEMP_HANDLE_PERIOD,
  TEMP_HANDLE_PERIOD_CHR_USR_DESCR,

  TEMP_HANDLE_END_PLUS_ONE
};

/**************************************************************************************************
 Macros
**************************************************************************************************/

/* Values for config attributes. */
#define TEMP_ATT_CONFIG_DISABLE  0x00u
#define TEMP_ATT_CONFIG_ENABLE   0x01u

/* Values for period attributes. */
#define TEMP_ATT_PERIOD_MAX      250u
#define TEMP_ATT_PERIOD_DEFAULT  100u
#define TEMP_ATT_PERIOD_MIN       10u

/* Sizes of attributes. */
#define TEMP_SIZE_CONFIG_ATT  1u
#define TEMP_SIZE_PERIOD_ATT  1u
#define TEMP_SIZE_DATA_ATT    6u

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcTempAddGroup(void);
void SvcTempRemoveGroup(void);
void SvcTempCbackRegister(attsWriteCback_t writeCback);

#endif /* SVC_TEMP_H */
