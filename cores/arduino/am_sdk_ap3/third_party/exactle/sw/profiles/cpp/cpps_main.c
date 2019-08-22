/*************************************************************************************************/
/*!
*  \file   cpps_main.c
*
*  \brief  Cycling Power Profile Sensor Implementation.
*
*          $Date: 2014-02-06 11:36:43 -0800 (Thu, 06 Feb 2014) $
*          $Revision: 1113 $
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

#include <string.h>
#include "wsf_types.h"
#include "wsf_assert.h"
#include "wsf_trace.h"
#include "bstream.h"
#include "dm_api.h"
#include "svc_cps.h"
#include "svc_ch.h"
#include "cpp_api.h"

/*************************************************************************************************
* Constant Definitions 
*************************************************************************************************/

/* The maximum length of a power measurement */
#define CPPS_PM_MAX_LEN     34

/* Power Measurement Flag Indicies */
enum
{
  CPPS_PPBP_FLAG_INDEX,         /* Pedal Power Balance Present */
  CPPS_PPBR_FLAG_INDEX,         /* Pedal Power Balance Reference */
  CPPS_ATP_FLAG_INDEX,          /* Accumulated Torque Present */
  CPPS_ATS_FLAG_INDEX,          /* Accumulated Torque Source */
  CPPS_WRDP_FLAG_INDEX,         /* Wheel Revolution Data Present */
  CPPS_CRDP_FLAG_INDEX,         /* Crank Revolution Data Present */
  CPPS_EFMP_FLAG_INDEX,         /* Extreme Force Magnitudes Present */
  CPPS_ETMP_FLAG_INDEX,         /* Extreme Torque Magnitudes Present */
  CPPS_EAP_FLAG_INDEX,          /* Extreme Angles Present */
  CPPS_TDSAP_FLAG_INDEX,        /* Top Dead Spot Angle Present */
  CPPS_BDSAP_FLAG_INDEX,        /* Bottom Dead Spot Angle Present */
  CPPS_AEP_FLAG_INDEX,          /* Accumulated Energy Present */
  CPPS_OCI_FLAG_INDEX,           /* Offset Compensation Indicator */
  CPPS_NUM_FLAGS
};

/**************************************************************************************************
Data Types
**************************************************************************************************/

/* Power Measurement Data */
typedef struct
{
  uint16_t  flags;                      /* Power Measurement Flags */
  uint16_t  insantaneousPower;          /* Instantaneous Power Measurement */
  uint8_t   powerBalance;               /* Pedal Power Balance */
  uint16_t  accumulatedTorque;          /* Accumulated Torque */
  uint32_t  wheelRevolutions;           /* Wheel Revolution */
  uint16_t  wheelEventTime;             /* Last Wheel Revolution Event Time */
  uint16_t  crankRevolutions;           /* Crank Revolution */
  uint16_t  crankEventTime;             /* Last Crank Revolution Event Time */
  uint16_t  maxForceMagnitude;          /* Max Extreme Force Magnitudes */
  uint16_t  minForceMagnitude;          /* Min Extreme Force Magnitudes */
  uint16_t  maxTorqueMagnitude;         /* Max Extreme Torque Magnitudes */
  uint16_t  minTorqueMagnitude;         /* Min Extreme Torque Magnitudes */
  uint16_t  maxAngle;                   /* Max Extreme Angles */
  uint16_t  minAngle;                   /* Min Extreme Angles */
  uint16_t  topDeadSpotAngle;           /* Top Dead Spot Angle */
  uint16_t  btmDeadSpotAngle;           /* Bottom Dead Spot Angle */
  uint16_t  accumulatedEnergy;          /* Accumulated Energy */
} cppPmData_t;

/**************************************************************************************************
Local Variables
**************************************************************************************************/

/* measurement data */
cppPmData_t cppsPmData;

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
void CppsSetSensorLocation(uint8_t location)
{
  AttsSetAttr(CPS_CPSL_HDL, sizeof(uint8_t), &location);
}

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
void CppsSetFeatures(uint32_t features)
{
  uint8_t tempData[4] = {UINT32_TO_BYTES(features)};
  AttsSetAttr(CPS_CPF_HDL, sizeof(tempData), tempData);
}

/*************************************************************************************************/
/*!
*  \fn     CppsSetParameter
*
*  \brief  Set a cycling power measurement parameter.
*
*  \param  type   Parameter identifier
*  \param  value  Measurement value.
*
*  \return none
*/
/*************************************************************************************************/
void CppsSetParameter(uint8_t type, uint32_t value)
{
  switch (type)
  {
  case CPP_PM_PARAM_INSTANTANEOUS_POWER:
    cppsPmData.insantaneousPower = (uint16_t) value;
    break;

  case CPP_PM_PARAM_PEDAL_POWER:
    cppsPmData.flags |= (1 << CPPS_PPBP_FLAG_INDEX);
    cppsPmData.powerBalance = (uint8_t) value;
    break;

  case CPP_PM_PARAM_ACCUMULATED_TORQUE:
    cppsPmData.flags |= (1 << CPPS_ATP_FLAG_INDEX);
    cppsPmData.accumulatedTorque = (uint16_t) value;
    break;

  case CPP_PM_PARAM_WHEEL_REVOLUTIONS:
    cppsPmData.flags |= (1 << CPPS_WRDP_FLAG_INDEX);
    cppsPmData.wheelRevolutions = value;
    break;

  case CPP_PM_PARAM_LAST_WHEEL_REV_TIME:
    cppsPmData.flags |= (1 << CPPS_WRDP_FLAG_INDEX);
    cppsPmData.wheelEventTime = (uint16_t) value;
    break;

  case CPP_PM_PARAM_CRANK_REVOLUTIONS:
    cppsPmData.flags |= (1 << CPPS_CRDP_FLAG_INDEX);
    cppsPmData.crankRevolutions = (uint16_t) value;
    break;

  case CPP_PM_PARAM_LAST_CRANK_TIME:
    cppsPmData.flags |= (1 << CPPS_CRDP_FLAG_INDEX);
    cppsPmData.crankEventTime = (uint16_t) value;
    break;

  case CPP_PM_PARAM_MAX_FORCE_MAGNITUDE:
    cppsPmData.flags |= (1 << CPPS_EFMP_FLAG_INDEX);
    cppsPmData.maxForceMagnitude = (uint16_t) value;
    break;

  case CPP_PM_PARAM_MIN_FORCE_MAGNITUDE:
    cppsPmData.flags |= (1 << CPPS_EFMP_FLAG_INDEX);
    cppsPmData.minForceMagnitude = (uint16_t) value;
    break;

  case CPP_PM_PARAM_MAX_TORQUE_MAGNITUDE:
    cppsPmData.flags |= (1 << CPPS_ETMP_FLAG_INDEX);
    cppsPmData.maxTorqueMagnitude = (uint16_t) value;
    break;

  case CPP_PM_PARAM_MIN_TORQUE_MAGNITUDE:
    cppsPmData.flags |= (1 << CPPS_ETMP_FLAG_INDEX);
    cppsPmData.minTorqueMagnitude = (uint16_t) value;
    break;

  case CPP_PM_PARAM_MAX_EXTREME_ANGLE:
    cppsPmData.flags |= (1 << CPPS_EAP_FLAG_INDEX);
    cppsPmData.maxAngle = (uint16_t) value;
    break;

  case CPP_PM_PARAM_MIN_EXTREME_ANGLE:
    cppsPmData.flags |= (1 << CPPS_EAP_FLAG_INDEX);
    cppsPmData.minAngle = (uint16_t) value;
    break;

  case CPP_PM_PARAM_TOP_DEAD_SPOT:
    cppsPmData.flags |= (1 << CPPS_TDSAP_FLAG_INDEX);
    cppsPmData.topDeadSpotAngle = (uint16_t) value;
    break;

  case CPP_PM_PARAM_BOTTOM_DEAD_SPOT:
    cppsPmData.flags |= (1 << CPPS_BDSAP_FLAG_INDEX);
    cppsPmData.btmDeadSpotAngle = (uint16_t) value;
    break;

  case CPP_PM_PARAM_ACCUMULATED_ENERGY:
    cppsPmData.flags |= (1 << CPPS_AEP_FLAG_INDEX);
    cppsPmData.accumulatedEnergy = (uint16_t) value;
    break;

  default:
    break;

  }
}

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
void CppsSendPowerMeasurement(dmConnId_t connId)
{
  int8_t i;
  uint16_t len;
  uint8_t msg[CPPS_PM_MAX_LEN];
  uint8_t *p = msg;
  uint32_t temp;

  /* Add manditory parameters */
  UINT16_TO_BSTREAM(p, cppsPmData.flags);
  UINT16_TO_BSTREAM(p, cppsPmData.insantaneousPower);

  /* Add optional parameters */
  for (i = 0; i < CPPS_NUM_FLAGS; i++)
  {
    if (cppsPmData.flags & (1 << i))
    {
      switch (i)
      {
      case CPPS_PPBP_FLAG_INDEX:
        UINT8_TO_BSTREAM(p, cppsPmData.powerBalance);
        break;
      case CPPS_ATP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, cppsPmData.accumulatedTorque);
        break;
      case CPPS_WRDP_FLAG_INDEX:
        UINT32_TO_BSTREAM(p, cppsPmData.wheelRevolutions);
        UINT16_TO_BSTREAM(p, cppsPmData.wheelEventTime);
        break;
      case CPPS_CRDP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, cppsPmData.crankRevolutions);
        UINT16_TO_BSTREAM(p, cppsPmData.crankEventTime);
        break;
      case CPPS_EFMP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, cppsPmData.maxForceMagnitude);
        UINT16_TO_BSTREAM(p, cppsPmData.minForceMagnitude);
        break;
      case CPPS_ETMP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, cppsPmData.maxTorqueMagnitude);
        UINT16_TO_BSTREAM(p, cppsPmData.minTorqueMagnitude);
        break;
      case CPPS_EAP_FLAG_INDEX:
        temp = ((uint32_t)cppsPmData.maxAngle & 0xfff) | ((uint32_t)cppsPmData.minAngle << 12);
        UINT24_TO_BSTREAM(p, temp);
        break;
      case CPPS_TDSAP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, cppsPmData.topDeadSpotAngle);
        break;
      case CPPS_BDSAP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, cppsPmData.btmDeadSpotAngle);
        break;
      case CPPS_AEP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, cppsPmData.accumulatedEnergy);
        break;
      default:
        break;
      }
    }
  }

  /* Calculate message length */
  len = (uint16_t) (p - msg);

  /* Transmit notification */
  AttsHandleValueNtf(connId, CPS_CPM_HDL, len, msg);

  /* Clear the measurement data */
  memset(&cppsPmData, 0, sizeof(cppsPmData));
}
