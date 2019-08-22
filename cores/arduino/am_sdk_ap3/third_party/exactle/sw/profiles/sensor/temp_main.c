/*************************************************************************************************/
/*!
 *  \file   temp_main.c
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

#include <stdlib.h>

#include "wsf_types.h"
#include "app_api.h"
#include "att_api.h"
#include "wsf_trace.h"
#include "bstream.h"

#include "temp_api.h"
#include "svc_temp.h"

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! Control block. */
static struct
{
  wsfTimer_t  measTimer;
  bool_t      measTimerStarted;
} tempCb;

/*************************************************************************************************/
/*!
 *  \fn       tempUpdateTimer
 *
 *  \brief    Update measurement timer.
 *
 *  \return   None.
 */
/*************************************************************************************************/
static void tempUpdateTimer(void)
{
  uint8_t  config;
  uint8_t *pConfig = NULL;
  uint8_t  period;
  uint8_t *pPeriod = NULL;
  uint16_t attLen = 0;

  /* Get config & period. */
  AttsGetAttr(TEMP_HANDLE_CONFIG, &attLen, &pConfig);
  if (pConfig == NULL)
  {
    WSF_TRACE_ERR0("temp: unable to read config");
    return;
  }
  config = *pConfig;
  AttsGetAttr(TEMP_HANDLE_PERIOD, &attLen, &pPeriod);
  if (pPeriod == NULL)
  {
    WSF_TRACE_ERR0("temp: unable to read period");
    return;
  }
  period = *pPeriod;
  if (period < TEMP_ATT_PERIOD_MIN)
  {
    period = TEMP_ATT_PERIOD_MIN;
  }

  if (config == TEMP_ATT_CONFIG_ENABLE)
  {
    if (!tempCb.measTimerStarted)
    {
      tempCb.measTimerStarted = TRUE;
      WsfTimerStartMs(&tempCb.measTimer, period * 10u);
    }
  }
  else
  {
    if (tempCb.measTimerStarted)
    {
      tempCb.measTimerStarted = FALSE;
      WsfTimerStop(&tempCb.measTimer);
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     tempWriteCback
 *
 *  \brief  ATTS write callback for temperature profile.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
static uint8_t tempWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                              uint16_t offset, uint16_t len, uint8_t *pValue,
                              attsAttr_t *pAttr)
{
  switch (handle)
  {
    case TEMP_HANDLE_CONFIG:
    {
      uint8_t config;

      /* Check attribute value. */
      if (len != 1)
      {
        return ATT_ERR_LENGTH;
      }
      config = *pValue;
      if ((config != TEMP_ATT_CONFIG_DISABLE) && (config != TEMP_ATT_CONFIG_ENABLE))
      {
        return ATT_ERR_RANGE;
      }

      /* Save value. */
      AttsSetAttr(TEMP_HANDLE_CONFIG, len, pValue);

      /* Enable or disable timer. */
      tempUpdateTimer();
      return ATT_SUCCESS;
    }
    case TEMP_HANDLE_PERIOD:
    {
      uint8_t period;

      if (len != 1)
      {
        return ATT_ERR_LENGTH;
      }
      period = *pValue;
      if ((period < TEMP_ATT_PERIOD_MIN) || (period > TEMP_ATT_PERIOD_MAX))
      {
        return ATT_ERR_RANGE;
      }
      AttsSetAttr(TEMP_HANDLE_PERIOD, len, pValue);
      return ATT_SUCCESS;
    }
  }
  return ATT_ERR_NOT_SUP;
}

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
void TempStart(wsfHandlerId_t handlerId, uint8_t timerEvt)
{
  SvcTempAddGroup();
  SvcTempCbackRegister(tempWriteCback);

  tempCb.measTimer.handlerId = handlerId;
  tempCb.measTimer.msg.event = timerEvt;
  tempCb.measTimerStarted    = FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     TempStop
 *
 *  \brief  Stop service.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TempStop(void)
{
  TempMeasStop();
  SvcTempRemoveGroup();
}

/*************************************************************************************************/
/*!
 *  \fn     TempMeasStop
 *
 *  \brief  Measurement stop handler.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TempMeasStop(void)
{
  tempCb.measTimerStarted = FALSE;
  WsfTimerStop(&tempCb.measTimer);
}

/*************************************************************************************************/
/*!
 *  \fn     TempMeasStart
 *
 *  \brief  Measurement start handler.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TempMeasStart(void)
{
  tempUpdateTimer();
}

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
void TempMeasComplete(dmConnId_t connId, int16_t temp)
{
  tempCb.measTimerStarted = FALSE;

  uint8_t tempData[2] = {UINT16_TO_BYTES(temp)};
  AttsSetAttr(TEMP_HANDLE_DATA, sizeof(tempData), tempData);
  AttsHandleValueNtf(connId, TEMP_HANDLE_DATA, sizeof(tempData), tempData);

  tempUpdateTimer();
}
