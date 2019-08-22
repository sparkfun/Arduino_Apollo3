/*************************************************************************************************/
/*!
 *  \file   gyro_main.c
 *
 *  \brief  Example gyroscope service profile.
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

#include <stdlib.h>

#include "wsf_types.h"
#include "app_api.h"
#include "att_api.h"
#include "wsf_trace.h"
#include "bstream.h"

#include "gyro_api.h"
#include "svc_gyro.h"

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! Control block. */
static struct
{
  wsfTimer_t  measTimer;
  bool_t      measTimerStarted;
} gyroCb;

/*************************************************************************************************/
/*!
 *  \fn       gyroUpdateTimer
 *
 *  \brief    Update measurement timer.
 *
 *  \return   None.
 */
/*************************************************************************************************/
static void gyroUpdateTimer(void)
{
  uint8_t  config;
  uint8_t *pConfig = NULL;
  uint8_t  period;
  uint8_t *pPeriod = NULL;
  uint16_t attLen = 0;

  /* Get config & period. */
  AttsGetAttr(GYRO_HANDLE_CONFIG, &attLen, &pConfig);
  if (pConfig == NULL)
  {
    WSF_TRACE_ERR0("gyro: unable to read config");
    return;
  }
  config = *pConfig;
  AttsGetAttr(GYRO_HANDLE_PERIOD, &attLen, &pPeriod);
  if (pPeriod == NULL)
  {
    WSF_TRACE_ERR0("gyro: unable to read period");
    return;
  }
  period = *pPeriod;
  if (period < GYRO_ATT_PERIOD_MIN)
  {
    period = GYRO_ATT_PERIOD_MIN;
  }

  if (config == GYRO_ATT_CONFIG_ENABLE)
  {
    if (!gyroCb.measTimerStarted)
    {
      gyroCb.measTimerStarted = TRUE;
      WsfTimerStartMs(&gyroCb.measTimer, period * 10u);
    }
  }
  else
  {
    if (gyroCb.measTimerStarted)
    {
      gyroCb.measTimerStarted = FALSE;
      WsfTimerStop(&gyroCb.measTimer);
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     gyroWriteCback
 *
 *  \brief  ATTS write callback for gyroscope profile.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
static uint8_t gyroWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                              uint16_t offset, uint16_t len, uint8_t *pValue,
                              attsAttr_t *pAttr)
{
  switch (handle)
  {
    case GYRO_HANDLE_CONFIG:
    {
      uint8_t config;

      /* Check attribute value. */
      if (len != 1)
      {
        return ATT_ERR_LENGTH;
      }
      config = *pValue;
      if ((config != GYRO_ATT_CONFIG_DISABLE) && (config != GYRO_ATT_CONFIG_ENABLE))
      {
        return ATT_ERR_RANGE;
      }

      /* Save value. */
      AttsSetAttr(GYRO_HANDLE_CONFIG, len, pValue);

      /* Enable or disable timer. */
      gyroUpdateTimer();
      return ATT_SUCCESS;
    }
    case GYRO_HANDLE_PERIOD:
    {
      uint8_t period;

      if (len != 1)
      {
        return ATT_ERR_LENGTH;
      }
      period = *pValue;
      if ((period < GYRO_ATT_PERIOD_MIN) || (period > GYRO_ATT_PERIOD_MAX))
      {
        return ATT_ERR_RANGE;
      }
      AttsSetAttr(GYRO_HANDLE_PERIOD, len, pValue);
      return ATT_SUCCESS;
    }
  }
  return ATT_ERR_NOT_SUP;
}

/*************************************************************************************************/
/*!
 *  \fn     GyroStart
 *
 *  \brief  Start service.
 *
 *  \param  handlerId       Handler ID.
 *  \param  timerEvt        Timer message event.
 *
 *  \return None.
 */
/*************************************************************************************************/
void GyroStart(wsfHandlerId_t handlerId, uint8_t timerEvt)
{
  SvcGyroAddGroup();
  SvcGyroCbackRegister(gyroWriteCback);

  gyroCb.measTimer.handlerId = handlerId;
  gyroCb.measTimer.msg.event = timerEvt;
  gyroCb.measTimerStarted    = FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     GyroStop
 *
 *  \brief  Stop service.
 *
 *  \return None.
 */
/*************************************************************************************************/
void GyroStop(void)
{
  gyroCb.measTimerStarted = FALSE;
  WsfTimerStop(&gyroCb.measTimer);

  SvcGyroRemoveGroup();
}

/*************************************************************************************************/
/*!
 *  \fn     GyroMeasStop
 *
 *  \brief  Measurement stop handler.
 *
 *  \return None.
 */
/*************************************************************************************************/
void GyroMeasStop(void)
{
  gyroCb.measTimerStarted = FALSE;
  WsfTimerStop(&gyroCb.measTimer);
}

/*************************************************************************************************/
/*!
 *  \fn     GyroMeasStart
 *
 *  \brief  Measurement start handler.
 *
 *  \return None.
 */
/*************************************************************************************************/
void GyroMeasStart(void)
{
  gyroUpdateTimer();
}

/*************************************************************************************************/
/*!
 *  \fn     GyroMeasComplete
 *
 *  \brief  Measurement complete handler.
 *
 *  \param  connId    Connection ID.
 *  \param  x         Gyroscope x-axis reading.
 *  \param  y         Gyroscope y-axis reading.
 *  \param  z         Gyroscope z-axis reading.
 *
 *  \return None.
 */
/*************************************************************************************************/
void GyroMeasComplete(dmConnId_t connId, int16_t x, int16_t y, int16_t z)
{
  gyroCb.measTimerStarted = FALSE;

  uint8_t gyroData[6] = {UINT16_TO_BYTES(x), UINT16_TO_BYTES(y), UINT16_TO_BYTES(z)};
  AttsSetAttr(GYRO_HANDLE_DATA, sizeof(gyroData), gyroData);
  AttsHandleValueNtf(connId, GYRO_HANDLE_DATA, sizeof(gyroData), gyroData);

  gyroUpdateTimer();
}
