/*************************************************************************************************/
/*!
*  \file   rscps_main.c
*
*  \brief  Running Speed and Cadence Profile Sensor Implementation.
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
#include "svc_rscs.h"
#include "svc_ch.h"
#include "rscp_api.h"

/*************************************************************************************************
* Constant Definitions 
*************************************************************************************************/

/* The maximum length of a power measurement */
#define RSCPS_PM_MAX_LEN     10

/* Running Speed Measurement Flag Indicies */
enum
{
  RSCPS_ISLP_FLAG_INDEX,          /* Instantaneous Stride Length Present Present */
  RSCPS_TDP_FLAG_INDEX,           /* Total Distance Present */
  RSCPS_WRS_FLAG_INDEX,           /* Walking or Running Status bits */
  RSCPS_NUM_FLAGS
};

/**************************************************************************************************
Data Types
**************************************************************************************************/

/* Running Speed Measurement Data */
typedef struct
{
  uint8_t   flags;                  /* Speed Measurement Flags */
  uint16_t  speed;                  /* Instantaneous Speed */
  uint8_t   cadence;                /* Instantaneous Cadence */
  uint16_t  stride;                 /* Instantaneous Stride Length */
  uint32_t  distance;               /* Total Distance */
} rscpSmData_t;

/**************************************************************************************************
Local Variables
**************************************************************************************************/

rscpSmData_t rscpSmData;

/*************************************************************************************************/
/*!
*  \fn     RscpsSetSensorLocation
*
*  \brief  Set the sensor location attribute.
*
*  \param  location   Sensor Location.
*
*  \return none
*/
/*************************************************************************************************/
void RscpsSetSensorLocation(uint8_t location)
{
  AttsSetAttr(RSCS_SL_HDL, sizeof(uint8_t), &location);
}

/*************************************************************************************************/
/*!
*  \fn     RscpsSetFeatures
*
*  \brief  Set the features attribute.
*
*  \param  features   Features bitmask.
*
*  \return none
*/
/*************************************************************************************************/
void RscpsSetFeatures(uint16_t features)
{
  uint8_t tempData[2] = {UINT16_TO_BYTES(features)};
  AttsSetAttr(RSCS_RSF_HDL, sizeof(tempData), tempData);
}

/*************************************************************************************************/
/*!
*  \fn     RscpsSetParameter
*
*  \brief  Set a running speed measurement parameter.
*
*  \param  type   Parameter identifier
*  \param  value  Measurement value.
*
*  \return none
*/
/*************************************************************************************************/
void RscpsSetParameter(uint8_t type, uint32_t value)
{
  switch (type)
  {
  case RSCP_SM_PARAM_SPEED:
    rscpSmData.speed = (uint16_t) value;
    break;

  case RSCP_SM_PARAM_CADENCE:
    rscpSmData.cadence = (uint8_t) value;
    break;

  case RSCP_SM_PARAM_STRIDE_LENGTH:
    rscpSmData.flags |= (1 << RSCPS_ISLP_FLAG_INDEX);
    rscpSmData.stride = (uint16_t) value;
    break;

  case RSCP_SM_PARAM_TOTAL_DISTANCE:
    rscpSmData.flags |= (1 << RSCPS_TDP_FLAG_INDEX);
    rscpSmData.distance = value;
    break;

  case RSCP_SM_PARAM_STATUS:
    if (value)
    {
      rscpSmData.flags |= (1 << RSCPS_WRS_FLAG_INDEX);
    }
    else
    {
      rscpSmData.flags &= ~(1 << RSCPS_WRS_FLAG_INDEX);
    }
    break;

  default:
    break;
  }
}

/*************************************************************************************************/
/*!
*  \fn     RscpsSendSpeedMeasurement
*
*  \brief  Notifies the collector of a Cycle Speed Measurement.
*
*  \param  connId  Connection ID
*
*  \return none
*/
/*************************************************************************************************/
void RscpsSendSpeedMeasurement(dmConnId_t connId)
{
  int8_t i;
  uint16_t len;
  uint8_t msg[RSCPS_PM_MAX_LEN];
  uint8_t *p = msg;

  /* Add manditory parameters */
  UINT8_TO_BSTREAM(p, rscpSmData.flags);
  UINT16_TO_BSTREAM(p, rscpSmData.speed);
  UINT8_TO_BSTREAM(p, rscpSmData.cadence);

  /* Add optional parameters */
  for (i = 0; i < RSCPS_NUM_FLAGS; i++)
  {
    if (rscpSmData.flags & (1 << i))
    {
      switch (i)
      {
      case RSCPS_ISLP_FLAG_INDEX:
        UINT16_TO_BSTREAM(p, rscpSmData.stride);
        break;
      case RSCPS_TDP_FLAG_INDEX:
        UINT32_TO_BSTREAM(p, rscpSmData.distance);
        break;

      default:
        break;
      }
    }
  }

  /* Calculate message length */
  len = (uint16_t) (p - msg);

  /* Transmit notification */
  AttsHandleValueNtf(connId, RSCS_RSM_HDL, len, msg);

  /* Clear the measurement data */
  memset(&rscpSmData, 0, sizeof(rscpSmData));
}
