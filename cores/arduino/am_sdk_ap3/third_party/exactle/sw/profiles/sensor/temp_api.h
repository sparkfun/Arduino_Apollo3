/*************************************************************************************************/
/*!
 *  \file   temp_api.h
 *
 *  \brief  Example temperature service profile.
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

#ifndef TEMP_API_H
#define TEMP_API_H

#include "wsf_types.h"
#include "wsf_os.h"

/*************************************************************************************************/
/*!
 *  \fn     TempStart
 *
 *  \brief  Start service.
 *
 *  \param  handlerId       Handler ID.
 *  \param  timerEvt        Timer message event.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TempStart(wsfHandlerId_t handlerId, uint8_t timerEvt);

/*************************************************************************************************/
/*!
 *  \fn     TempStop
 *
 *  \brief  Stop service.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TempStop(void);

/*************************************************************************************************/
/*!
 *  \fn     TempMeasStop
 *
 *  \brief  Measurement stop handler.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TempMeasStop(void);

/*************************************************************************************************/
/*!
 *  \fn     TempMeasStart
 *
 *  \brief  Measurement start handler.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TempMeasStart(void);

/*************************************************************************************************/
/*!
 *  \fn     TempMeasComplete
 *
 *  \brief  Measurement complete handler.
 *
 *  \param  connId    Connection ID.
 *  \param  temp      Temperature reading.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TempMeasComplete(dmConnId_t connId, int16_t temp);

#endif /* TEMP_API_H */
