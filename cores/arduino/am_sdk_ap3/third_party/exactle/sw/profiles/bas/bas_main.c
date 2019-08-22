/*************************************************************************************************/
/*!
 *  \file   bas_main.c
 *
 *  \brief  Battery service server.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2012-2017 ARM Ltd., all rights reserved.
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

#include <string.h>
#include "wsf_types.h"
#include "wsf_assert.h"
#include "wsf_trace.h"
#include "bstream.h"
#include "att_api.h"
#include "svc_ch.h"
#include "svc_batt.h"
#include "app_api.h"
#include "app_hw.h"
#include "bas_api.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/* battery level initialization value */
#define BAS_BATT_LEVEL_INIT           0xFF

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/* Connection control block */
typedef struct
{
  dmConnId_t    connId;               /* Connection ID */
  bool_t        battToSend;           /* battery measurement ready to be sent on this channel */
  uint8_t       sentBattLevel;        /* value of last sent battery level */
} basConn_t;

/* Control block */
static struct
{
  basConn_t         conn[DM_CONN_MAX];    /* connection control block */
  wsfTimer_t        measTimer;            /* periodic measurement timer */
  basCfg_t          cfg;                  /* configurable parameters */
  uint16_t          currCount;            /* current measurement period count */
  bool_t            txReady;              /* TRUE if ready to send notifications */
  uint8_t           measBattLevel;        /* value of last measured battery level */
} basCb;

/*************************************************************************************************/
/*!
 *  \fn     basNoConnActive
 *        
 *  \brief  Return TRUE if no connections with active measurements.
 *
 *  \return TRUE if no connections active.
 */
/*************************************************************************************************/
static bool_t basNoConnActive(void)
{
  basConn_t     *pConn = basCb.conn;
  uint8_t       i;
  
  for (i = 0; i < DM_CONN_MAX; i++, pConn++)
  {
    if (pConn->connId != DM_CONN_ID_NONE)
    {      
      return FALSE;
    }
  }
  return TRUE;
}

/*************************************************************************************************/
/*!
 *  \fn     basSetupToSend
 *        
 *  \brief  Setup to send measurements on active connections.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void basSetupToSend(void)
{
  basConn_t     *pConn = basCb.conn;
  uint8_t       i;
  
  for (i = 0; i < DM_CONN_MAX; i++, pConn++)
  {
    if (pConn->connId != DM_CONN_ID_NONE)
    {      
      pConn->battToSend = TRUE;
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     basFindNextToSend
 *        
 *  \brief  Find next connection with measurement to send.
 *
 *  \param  cccIdx  Battery measurement CCC descriptor index. 
 *
 *  \return Connection control block.
 */
/*************************************************************************************************/
static basConn_t *basFindNextToSend(uint8_t cccIdx)
{
  basConn_t    *pConn = basCb.conn;
  uint8_t       i;
  
  for (i = 0; i < DM_CONN_MAX; i++, pConn++)
  {
    if (pConn->connId != DM_CONN_ID_NONE && pConn->battToSend &&
        pConn->sentBattLevel != basCb.measBattLevel)
    {
      if (AttsCccEnabled(pConn->connId, cccIdx))
      {
        return pConn;
      }
    }
  }
  return NULL;
}

/*************************************************************************************************/
/*!
 *  \fn     basSendPeriodicBattlevel
 *        
 *  \brief  Send periodic battery measurement.
 *
 *  \param  pConn   Connection control block.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void basSendPeriodicBattlevel(basConn_t *pConn)
{
  BasSendBattLevel(pConn->connId, basCb.measTimer.msg.status, basCb.measBattLevel);
  pConn->sentBattLevel = basCb.measBattLevel;
  pConn->battToSend = FALSE;
  basCb.txReady = FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     basConnOpen
 *        
 *  \brief  Handle connection open.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void basConnOpen(dmEvt_t *pMsg)
{
  basCb.txReady = TRUE;
}

/*************************************************************************************************/
/*!
 *  \fn     basHandleValueCnf
 *        
 *  \brief  Handle a received ATT handle value confirm.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void basHandleValueCnf(attEvt_t *pMsg)
{
  basConn_t  *pConn;

  if (pMsg->hdr.status == ATT_SUCCESS && pMsg->handle == BATT_LVL_HDL)
  {
    basCb.txReady = TRUE;
  
    /* find next connection to send (note ccc idx is stored in timer status) */
    if ((pConn = basFindNextToSend(basCb.measTimer.msg.status)) != NULL)
    {
      basSendPeriodicBattlevel(pConn);
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     basMeasTimerExp
 *        
 *  \brief  This function is called by the application when the periodic measurement
 *          timer expires.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void basMeasTimerExp(wsfMsgHdr_t *pMsg)
{
  basConn_t  *pConn;

  /* if there are active connections */
  if (basNoConnActive() == FALSE)
  {
    if (--basCb.currCount == 0)
    {
      /* reset count */
      basCb.currCount = basCb.cfg.count;
      
      /* set up battery measurement to be sent on all connections */
      basSetupToSend();

      /* read battery measurement sensor data */
      AppHwBattRead(&basCb.measBattLevel);
    
      /* if ready to send measurements */
      if (basCb.txReady)
      {
        /* find next connection to send (note ccc idx is stored in timer status) */
        if ((pConn = basFindNextToSend(pMsg->status)) != NULL)
        {
          basSendPeriodicBattlevel(pConn);
        }
      }
    }
        
    /* restart timer */
    WsfTimerStartSec(&basCb.measTimer, basCb.cfg.period);    
  }
}

/*************************************************************************************************/
/*!
 *  \fn     BasInit
 *        
 *  \brief  Initialize the battery service server.
 *
 *  \param  handerId    WSF handler ID of the application using this service.
 *  \param  pCfg        Battery service configurable parameters.
 *
 *  \return None.
 */
/*************************************************************************************************/
void BasInit(wsfHandlerId_t handlerId, basCfg_t *pCfg)
{
  basCb.measTimer.handlerId = handlerId;
  basCb.cfg = *pCfg;
}

/*************************************************************************************************/
/*!
 *  \fn     BasMeasBattStart
 *        
 *  \brief  Start periodic battery level measurement.  This function starts a timer to perform
 *          periodic battery measurements.
 *
 *  \param  connId      DM connection identifier.
 *  \param  timerEvt    WSF event designated by the application for the timer.
 *  \param  battCccIdx  Index of battery level CCC descriptor in CCC descriptor handle table.
 *
 *  \return None.
 */
/*************************************************************************************************/
void BasMeasBattStart(dmConnId_t connId, uint8_t timerEvt, uint8_t battCccIdx)
{
  /* if this is first connection */
  if (basNoConnActive())
  {   
    /* initialize control block */
    basCb.measTimer.msg.event = timerEvt;
    basCb.measTimer.msg.status = battCccIdx;  
    basCb.measBattLevel = BAS_BATT_LEVEL_INIT;
    basCb.currCount = basCb.cfg.count;
    
    /* start timer */
    WsfTimerStartSec(&basCb.measTimer, basCb.cfg.period);
  }
  
  /* set conn id and last sent battery level */
  basCb.conn[connId - 1].connId = connId;  
  basCb.conn[connId - 1].sentBattLevel = BAS_BATT_LEVEL_INIT;
}

/*************************************************************************************************/
/*!
 *  \fn     BasMeasBattStop
 *        
 *  \brief  Stop periodic battery level measurement.
 *
 *  \param  connId      DM connection identifier.
 *
 *  \return None.
 */
/*************************************************************************************************/
void BasMeasBattStop(dmConnId_t connId)
{
  /* clear connection */
  basCb.conn[connId - 1].connId = DM_CONN_ID_NONE;
  basCb.conn[connId - 1].battToSend = FALSE;
  
  /* if no remaining connections */
  if (basNoConnActive())
  {
    /* stop timer */
    WsfTimerStop(&basCb.measTimer);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     BasProcMsg
 *        
 *  \brief  Process received WSF message.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void BasProcMsg(wsfMsgHdr_t *pMsg)
{
  if (pMsg->event == DM_CONN_OPEN_IND)
  {
    basConnOpen((dmEvt_t *) pMsg);
  }
  else if (pMsg->event == ATTS_HANDLE_VALUE_CNF)
  {
    basHandleValueCnf((attEvt_t *) pMsg);
  }
  else if (pMsg->event == basCb.measTimer.msg.event)
  {
    basMeasTimerExp(pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     BasSendBattLevel
 *        
 *  \brief  Send the battery level to the peer device.
 *
 *  \param  connId      DM connection identifier.
 *  \param  idx         Index of battery level CCC descriptor in CCC descriptor handle table. 
 *  \param  level       The battery level.
 *
 *  \return None.
 */
/*************************************************************************************************/
void BasSendBattLevel(dmConnId_t connId, uint8_t idx, uint8_t level)
{
  if (AttsCccEnabled(connId, idx))
  {
    AttsHandleValueNtf(connId, BATT_LVL_HDL, CH_BATT_LEVEL_LEN, &level);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     BasReadCback
 *        
 *  \brief  ATTS read callback for battery service used to read the battery level.  Use this
 *          function as a parameter to SvcBattCbackRegister().
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t BasReadCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                     uint16_t offset, attsAttr_t *pAttr)
{
  /* read the battery level and set attribute value */
  AppHwBattRead(pAttr->pValue);
  
  return ATT_SUCCESS;
}
