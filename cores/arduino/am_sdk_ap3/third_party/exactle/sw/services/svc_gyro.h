/*************************************************************************************************/
/*!
 *  \file   svc_gyro.h
 *
 *  \brief  Example gyroscope sensor service implementation.
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

#ifndef SVC_GYRO_H
#define SVC_GYRO_H

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

#define GYRO_HANDLE_START  0x50
#define GYRO_HANDLE_END   (GYRO_HANDLE_END_PLUS_ONE - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* Gyroscope service handles. */
enum
{
  GYRO_HANDLE_SVC = GYRO_HANDLE_START,

  GYRO_HANDLE_DATA_CHR,
  GYRO_HANDLE_DATA,
  GYRO_HANDLE_DATA_CLIENT_CHR_CONFIG,
  GYRO_HANDLE_DATA_CHR_USR_DESCR,

  GYRO_HANDLE_TEMPDATA_CHR,
  GYRO_HANDLE_TEMPDATA,
  GYRO_HANDLE_TEMPDATA_CLIENT_CHR_CONFIG,
  GYRO_HANDLE_TEMPDATA_CHR_USR_DESCR,

  GYRO_HANDLE_CONFIG_CHR,
  GYRO_HANDLE_CONFIG,
  GYRO_HANDLE_CONFIG_CHR_USR_DESCR,

  GYRO_HANDLE_PERIOD_CHR,
  GYRO_HANDLE_PERIOD,
  GYRO_HANDLE_PERIOD_CHR_USR_DESCR,

  GYRO_HANDLE_END_PLUS_ONE
};

/**************************************************************************************************
 Macros
**************************************************************************************************/

/* Values for config attributes. */
#define GYRO_ATT_CONFIG_DISABLE  0x00u
#define GYRO_ATT_CONFIG_ENABLE   0x01u

/* Values for period attributes. */
#define GYRO_ATT_PERIOD_MAX      250u
#define GYRO_ATT_PERIOD_DEFAULT  100u
#define GYRO_ATT_PERIOD_MIN       10u

/* Sizes of attributes. */
#define GYRO_SIZE_CONFIG_ATT       1u
#define GYRO_SIZE_PERIOD_ATT       1u
#define GYRO_SIZE_DATA_ATT         6u
#define GYRO_SIZE_TEMPDATA_ATT     2u

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcGyroAddGroup(void);
void SvcGyroRemoveGroup(void);
void SvcGyroCbackRegister(attsWriteCback_t writeCback);

#endif /* SVC_GYRO_H */
