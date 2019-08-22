/*************************************************************************************************/
/*!
*  \file   cscps_main.c
*
*  \brief  Cycling Speed and Cadence Profile Sensor Implementation.
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
#include "svc_cscs.h"
#include "svc_ch.h"
#include "cscp_api.h"

/*************************************************************************************************
* Constant Definitions 
*************************************************************************************************/

/* The maximum length of a power measurement */
#define CSCPS_PM_MAX_LEN          11

/* Cycle Speed and Cadence Measurement Flag Indicies */
enum
{
  CSCPS_WRDP_FLAG_INDEX,          /* Wheel Revolution Data Present */
  CSCPS_CRDP_FLAG_INDEX,          /* Crank Revolution Data Present */
  CSCPS_NUM_FLAGS
};

/**************************************************************************************************
Data Types
**************************************************************************************************/

/* Cycle Speed Measurement Data */
typedef struct
{
  uint8_t   flags;                  /* Speed Measurement Flags */
  uint32_t  wheelRevs;              /* Cumulative Wheel Revolutions */
  uint16_t  lastWheelEventTime;     /* Last Wheel Event Time */
  uint16_t  crankRevs;              /* Cumulative Crank Revolutions */
  uint16_t  lastCrankEventTime;     /* Last Crank Event Time */
} cscpSmData_t;

/**************************************************************************************************
Local Variables
**************************************************************************************************/

/* measurement data */
cscpSmData_t cscpsSmData;

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
void CscpsSetParameter(uint8_t type, uint32_t value)
{
  switch (type)
  {
    case CSCP_SM_PARAM_WHEEL_REVOLUTIONS:
      cscpsSmData.flags |= (1 << CSCPS_WRDP_FLAG_INDEX);
      cscpsSmData.wheelRevs = value;
      break;

    case CSCP_SM_PARAM_LAST_WHEEL_EVT_TIME:
      cscpsSmData.flags |= (1 << CSCPS_WRDP_FLAG_INDEX);
      cscpsSmData.lastWheelEventTime = (uint16_t) value;
      break;

    case CSCP_SM_PARAM_CRANK_REVOLUTIONS:
      cscpsSmData.flags |= (1 << CSCPS_CRDP_FLAG_INDEX);
      cscpsSmData.crankRevs = (uint16_t) value;
      break;

    case CSCP_SM_PARAM_LAST_CRANK_TIME:
      cscpsSmData.flags |= (1 << CSCPS_CRDP_FLAG_INDEX);
      cscpsSmData.lastCrankEventTime = (uint16_t) value;
      break;

    default:
      break;
  }
}

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
void CscpsSetSensorLocation(uint8_t location)
{
  AttsSetAttr(CSCS_SL_HDL, sizeof(uint8_t), &location);
}

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
void CscpsSetFeatures(uint16_t features)
{
  uint8_t tempData[2] = {UINT16_TO_BYTES(features)};
  AttsSetAttr(CSCS_CSF_HDL, sizeof(tempData), tempData);
}

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
void CscpsSendSpeedMeasurement(dmConnId_t connId)
{
  int8_t i;
  uint16_t len;
  uint8_t msg[CSCPS_PM_MAX_LEN];
  uint8_t *p = msg;

  /* Add manditory parameters */
  UINT8_TO_BSTREAM(p, cscpsSmData.flags);

  /* Add optional parameters */
  for (i = 0; i < CSCPS_NUM_FLAGS; i++)
  {
    if (cscpsSmData.flags & (1 << i))
    {
      switch (i)
      {
      case CSCPS_WRDP_FLAG_INDEX:
        UINT32_TO_BSTREAM(p, cscpsSmData.wheelRevs);
        UINT16_TO_BSTREAM(p, cscpsSmData.lastWheelEventTime);
        break;
      case CSCPS_CRDP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, cscpsSmData.crankRevs);
        UINT16_TO_BSTREAM(p, cscpsSmData.lastCrankEventTime);
        break;

      default:
        break;
     }
    }
  }

  /* Calculate message length */
  len = (uint16_t) (p - msg);

  /* Transmit notification */
  AttsHandleValueNtf(connId, CSCS_CSM_HDL, len, msg);

  /* Clear the measurement data */
  memset(&cscpsSmData, 0, sizeof(cscpsSmData));
}
