/*************************************************************************************************/
/*!
*  \file   cpp_api.h
*
*  \brief  Cycling Power Profile API.
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

#ifndef CPP_API_H
#define CPP_API_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
Macros
**************************************************************************************************/

/* Cycle Power Measurement Parameter Types */
#define CPP_PM_PARAM_INSTANTANEOUS_POWER            0     /* Instantaneous Power Measurement */
#define CPP_PM_PARAM_PEDAL_POWER                    1     /* Pedal Power Balance */
#define CPP_PM_PARAM_ACCUMULATED_TORQUE             2     /* Accumulated Torque */
#define CPP_PM_PARAM_WHEEL_REVOLUTIONS              3     /* Wheel Revolution */
#define CPP_PM_PARAM_LAST_WHEEL_REV_TIME            4     /* Last Wheel Revolution Event Time */
#define CPP_PM_PARAM_CRANK_REVOLUTIONS              5     /* Crank Revolution */
#define CPP_PM_PARAM_LAST_CRANK_TIME                6     /* Last Crank Revolution Event Time */
#define CPP_PM_PARAM_MAX_FORCE_MAGNITUDE            7     /* Max Extreme Force Magnitudes */
#define CPP_PM_PARAM_MIN_FORCE_MAGNITUDE            8     /* Min Extreme Force Magnitudes */
#define CPP_PM_PARAM_MAX_TORQUE_MAGNITUDE           9     /* Max Extreme Torque Magnitudes */
#define CPP_PM_PARAM_MIN_TORQUE_MAGNITUDE           10    /* Min Extreme Torque Magnitudes */
#define CPP_PM_PARAM_MAX_EXTREME_ANGLE              11    /* Max Extreme Angles */
#define CPP_PM_PARAM_MIN_EXTREME_ANGLE              12    /* Min Extreme Angles */
#define CPP_PM_PARAM_TOP_DEAD_SPOT                  13    /* Top Dead Spot Angle */
#define CPP_PM_PARAM_BOTTOM_DEAD_SPOT               14    /* Bottom Dead Spot Angle */
#define CPP_PM_PARAM_ACCUMULATED_ENERGY             15    /* Accumulated Energy */

/**************************************************************************************************
Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
*  \fn     CppsSendPowerMeasurement
*
*  \brief  Notifies the collector of a Cycle Power Measurement.
*
*  \param  connId  Connection ID
*
*  \return none
*/
/*************************************************************************************************/
void CppsSendPowerMeasurement(dmConnId_t connId);

/*************************************************************************************************/
/*!
*  \fn     CppsSetParameter
*
*  \brief  Set a cycle measurement parameter.
*
*  \param  type   Parameter identifier
*  \param  value  Measurement value.
*
*  \return none
*/
/*************************************************************************************************/
void CppsSetParameter(uint8_t type, uint32_t value);

/*************************************************************************************************/
/*!
*  \fn     CppsSetSensorLocation
*
*  \brief  Set the sensor location attribute.
*
*  \param  location   Sensor Location.
*
*  \return none
*/
/*************************************************************************************************/
void CppsSetSensorLocation(uint8_t location);

/*************************************************************************************************/
/*!
*  \fn     CppsSetFeatures
*
*  \brief  Set the features attribute.
*
*  \param  features   Features bitmask.
*
*  \return none
*/
/*************************************************************************************************/
void CppsSetFeatures(uint32_t features);

#ifdef __cplusplus
};
#endif

#endif /* CPP_API_H */
