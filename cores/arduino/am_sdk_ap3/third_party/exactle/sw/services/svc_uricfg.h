/*************************************************************************************************/
/*!
 *  \file   svc_uricfg.h
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2011-2017 ARM Ltd., all rights reserved.
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

#ifndef SVC_URICFG_H
#define SVC_URICFG_H

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

#define URICFG_HANDLE_START  0x80
#define URICFG_HANDLE_END   (URICFG_HANDLE_END_PLUS_ONE - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* UriBeacon configuration service handles */
enum
{
  URICFG_HANDLE_SVC = URICFG_HANDLE_START,

  URICFG_HANDLE_LOCKSTATE_CHR,
  URICFG_HANDLE_LOCKSTATE,
  URICFG_HANDLE_LOCKSTATE_CHR_USR_DESCR,

  URICFG_HANDLE_LOCK_CHR,
  URICFG_HANDLE_LOCK,
  URICFG_HANDLE_LOCK_CHR_USR_DESCR,

  URICFG_HANDLE_UNLOCK_CHR,
  URICFG_HANDLE_UNLOCK,
  URICFG_HANDLE_UNLOCK_CHR_USR_DESCR,

  URICFG_HANDLE_URIDATA_CHR,
  URICFG_HANDLE_URIDATA,
  URICFG_HANDLE_URIDATA_CHR_USR_DESCR,

  URICFG_HANDLE_URIFLAGS_CHR,
  URICFG_HANDLE_URIFLAGS,
  URICFG_HANDLE_URIFLAGS_CHR_USR_DESCR,

  URICFG_HANDLE_TXPWRLEVELS_CHR,
  URICFG_HANDLE_TXPWRLEVELS,
  URICFG_HANDLE_TXPWRLEVELS_CHR_USR_DESCR,

  URICFG_HANDLE_TXPWRMODE_CHR,
  URICFG_HANDLE_TXPWRMODE,
  URICFG_HANDLE_TXPWRMODE_CHR_USR_DESCR,

  URICFG_HANDLE_BEACONPERIOD_CHR,
  URICFG_HANDLE_BEACONPERIOD,
  URICFG_HANDLE_BEACONPERIOD_CHR_USR_DESCR,

  URICFG_HANDLE_RESET_CHR,
  URICFG_HANDLE_RESET,
  URICFG_HANDLE_RESET_CHR_USR_DESCR,

  URICFG_HANDLE_END_PLUS_ONE
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcUriCfgAddGroup(void);
void SvcUriCfgRemoveGroup(void);
void SvcUriCfgCbackRegister(attsWriteCback_t writeCback);

#endif /* SVC_URICFG_H */
