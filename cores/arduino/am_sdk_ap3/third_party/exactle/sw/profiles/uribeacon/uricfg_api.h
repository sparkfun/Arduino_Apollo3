/*************************************************************************************************/
/*!
 *  \file   uricfg_api.h
 *
 *  \brief  UriBeacon configuration profile.
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

#ifndef URICFG_API_H
#define URICFG_API_H

#include "wsf_types.h"
#include "att_api.h"
#include "svc_uricfg.h"
#include "uricfg_defs.h"

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     uriCfgAttWriteCback_t
 *
 *  \brief  Attribute write callback.
 *
 *  \param  handle      Attribute handle.
 *  \param  valueLen    Length of value data.
 *  \param  pValue      Pointer to value data.
 *
 *  \return None.
 */
/*************************************************************************************************/
typedef void (*uriCfgAttWriteCback_t)(uint16_t handle, uint16_t valueLen, const uint8_t *pValue);

/*************************************************************************************************/
/*!
 *  \fn     uriCfgLockChangeCback_t
 *
 *  \brief  Lock change callback.
 *
 *  \param  lockState   New lock state.
 *  \param  lock        Lock value.
 *
 *  \return None.
 */
/*************************************************************************************************/
typedef void (*uriCfgLockChangeCback_t)(uint8_t lockState, const uint8_t *pLock);

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     UriCfgStart
 *
 *  \brief  Start UriBeacon configuration service.
 *
 *  \param  pUriData          Initial URI data value
 *  \param  uriDataLen        Length of URI data value
 *  \param  uriFlags          Initial URI flags value
 *  \param  advTxPwrLevels    Initial advertised tx power levels value
 *  \param  txPwrMode         Initial tx power mode value
 *  \param  beaconPeriod      Initial beacon period value
 *
 *  \return None.
 */
/*************************************************************************************************/
void UriCfgStart(const uint8_t *pUriData, uint8_t uriDataLen, uint8_t uriFlags,
    int8_t *pAdvTxPwrLevels, uint8_t txPwrMode, uint16_t beaconPeriod);

/*************************************************************************************************/
/*!
 *  \fn     UriCfgStop
 *
 *  \brief  Stop UriBeacon configuration service.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UriCfgStop(void);

/*************************************************************************************************/
/*!
 *  \fn     UriCfgAttWriteCbackRegister
 *
 *  \brief  Register callback for written UriBeacon attributes.
 *
 *  \param  cback       Callback to invoke when an attribute changes.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UriCfgAttWriteCbackRegister(uriCfgAttWriteCback_t cback);

/*************************************************************************************************/
/*!
 *  \fn     UriCfgMakeLockable
 *
 *  \brief  Make UriBeacon lockable.
 *
 *  \param  lockState   Initial lock state value.
 *  \param  lock        Initial lock value.
 *  \param  cback       Callback to invoke when lock changes.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UriCfgMakeLockable(uint8_t lockState, uint8_t *pLock, uriCfgLockChangeCback_t cback);

/*************************************************************************************************/
/*!
 *  \fn     UriCfgSetUriDataResetValue
 *
 *  \brief  Set reset value of URI data.
 *
 *  \param  pUriData    Reset value of URI data.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UriCfgSetUriDataResetValue(const uint8_t *pUriData);

#endif /* URICFG_API_H */
