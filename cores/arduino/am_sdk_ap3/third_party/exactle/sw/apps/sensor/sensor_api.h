/*************************************************************************************************/
/*!
 *  \file   sensor_main.c
 *
 *  \brief  Sensor sample application.
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

#ifndef SENSOR_API_H
#define SENSOR_API_H

#include "wsf_types.h"
#include "wsf_os.h"

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     SensorStart
 *
 *  \brief  Start sensor application.
*
 *  \return None.
 */
/*************************************************************************************************/
void SensorStart(void);

/*************************************************************************************************/
/*!
 *  \fn     SensorHandler
 *
 *  \brief  WSF event handler for application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SensorHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);

/*************************************************************************************************/
/*!
 *  \fn     SensorHandlerInit
 *
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SensorHandlerInit(wsfHandlerId_t handlerId);

/**************************************************************************************************
  Application Functions
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     AppReadGyro
 *
 *  \brief  Read gyroscope sensor.
 *
 *  \param  pX        Storage for gyroscope x-axis reading.
 *  \param  pY        Storage for gyroscope y-axis reading.
 *  \param  pZ        Storage for gyroscope z-axis reading.
 *
 *  \return None.
 */
/*************************************************************************************************/
bool_t AppReadGyro(int16_t *pX, int16_t *pY, int16_t *pZ);

/*************************************************************************************************/
/*!
 *  \fn     AppReadTemp
 *
 *  \brief  Read temperature sensor.
 *
 *  \param  pTemp     Storage for temperature reading.
 *
 *  \return None.
 */
/*************************************************************************************************/
bool_t AppReadTemp(int16_t *pTemp);

#endif /* SENSOR_API_H */
