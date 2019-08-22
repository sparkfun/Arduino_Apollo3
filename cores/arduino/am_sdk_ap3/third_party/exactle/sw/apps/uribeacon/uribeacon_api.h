/*************************************************************************************************/
/*!
 *  \file   uribeacon_api.h
 *
 *  \brief  UriBeacon sample application.
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

#ifndef URIBEACON_API_H
#define URIBEACON_API_H

#include "wsf_types.h"
#include "wsf_os.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! UriBeacon parameter IDs */
enum
{
  URI_BEACON_PARAM_LOCK_STATE                 = 4,  /*!< lock state (for UriBeacon) [1 byte] */
  URI_BEACON_PARAM_URI_DATA                   = 5,  /*!< URI data (for UriBeacon) [18 bytes] */
  URI_BEACON_PARAM_URI_FLAGS                  = 6,  /*!< URI flags (for UriBeacon) [1 byte] */
  URI_BEACON_PARAM_ADVERTISED_TX_POWER_LEVELS = 7,  /*!< advertised tx power levels (for UriBeacon) [4 bytes] */
  URI_BEACON_PARAM_TX_POWER_MODE              = 8,  /*!< tx power mode (for UriBeacon) [1 byte] */
  URI_BEACON_PARAM_BEACON_PERIOD              = 9,  /*!< beacon period (for beacon) [2 bytes] */
  URI_BEACON_PARAM_LOCK                       = 10  /*!< lock [16 bytes] */
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     UriBeaconStart
 *
 *  \brief  Start UriBeacon application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UriBeaconStart(void);

/*************************************************************************************************/
/*!
 *  \fn     UriBeaconHandler
 *
 *  \brief  WSF event handler for application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UriBeaconHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);

/*************************************************************************************************/
/*!
 *  \fn     UriBeaconHandlerInit
 *
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UriBeaconHandlerInit(wsfHandlerId_t handlerId);

#endif /* URIBEACON_API_H */
