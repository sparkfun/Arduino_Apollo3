/*************************************************************************************************/
/*!
*  \file   cscp_api.h
*
*  \brief  Cycling Speed and Cadence Profile API.
*
*          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
*          $Revision: 10805 $
*
*  Copyright (c) 2016-2017 ARM Ltd., all rights reserved.
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

#ifndef CSCP_API_H
#define CSCP_API_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
Macros
**************************************************************************************************/

/* Cycling Speed Measurement Parameter Types */
#define CSCP_SM_PARAM_WHEEL_REVOLUTIONS             0     /* Cumulative Wheel Revolutions */
#define CSCP_SM_PARAM_LAST_WHEEL_EVT_TIME           1     /* Last Wheel Event Time */
#define CSCP_SM_PARAM_CRANK_REVOLUTIONS             2     /* Cumulative Crank Revolutions */
#define CSCP_SM_PARAM_LAST_CRANK_TIME               3     /* Last Crank Event Time */

/**************************************************************************************************
Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
*  \fn     CscpsSetParameter
*
*  \brief  Set a cycling speed measurement parameter.
*
*  \param  type   Parameter identifier
*  \param  value  Measurement value.
*
*  \return none
*/
/*************************************************************************************************/
void CscpsSetParameter(uint8_t type, uint32_t value);

/*************************************************************************************************/
/*!
*  \fn     CscpsSendSpeedMeasurement
*
*  \brief  Notifies the collector of a Cycle Speed Measurement.
*
*  \param  connId  Connection ID
*
*  \return none
*/
/*************************************************************************************************/
void CscpsSendSpeedMeasurement(dmConnId_t connId);

/*************************************************************************************************/
/*!
*  \fn     CscpsSetSensorLocation
*
*  \brief  Set the sensor location attribute.
*
*  \param  location   Sensor Location.
*
*  \return none
*/
/*************************************************************************************************/
void CscpsSetSensorLocation(uint8_t location);

/*************************************************************************************************/
/*!
*  \fn     CscpsSetFeatures
*
*  \brief  Set the features attribute.
*
*  \param  features   Features bitmask.
*
*  \return none
*/
/*************************************************************************************************/
void CscpsSetFeatures(uint16_t features);

#ifdef __cplusplus
};
#endif

#endif /* CSCP_API_H */
