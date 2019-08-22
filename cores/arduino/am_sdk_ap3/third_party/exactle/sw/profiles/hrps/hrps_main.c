/*************************************************************************************************/
/*!
 *  \file   hrps_main.c
 *
 *  \brief  Heart Rate profile sensor.
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
#include "svc_hrs.h"
#include "app_api.h"
#include "app_hw.h"
#include "hrps_api.h"

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/* Connection control block */
typedef struct
{
  dmConnId_t    connId;               /* Connection ID */
  bool_t        hrmToSend;            /* heart rate measurement ready to be sent on this channel */
} hrpsConn_t;

/* Control block */
static struct
{
  hrpsConn_t    conn[DM_CONN_MAX];    /* connection control block */
  wsfTimer_t    measTimer;            /* periodic measurement timer */
  appHrm_t      hrm;                  /* heart rate measurement */
  hrpsCfg_t     cfg;                  /* configurable parameters */
  uint16_t      energyExp;            /* energy expended value */
  bool_t        txReady;              /* TRUE if ready to send notifications */
  uint8_t       flags;                /* heart rate measurement flags */
} hrpsCb;

/*************************************************************************************************/
/*!
 *  \fn     hrpsNoConnActive
 *        
 *  \brief  Return TRUE if no connections with active measurements.
 *
 *  \return TRUE if no connections active.
 */
/*************************************************************************************************/
static bool_t hrpsNoConnActive(void)
{
  hrpsConn_t    *pConn = hrpsCb.conn;
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
 *  \fn     hrpsSetupToSend
 *        
 *  \brief  Setup to send measurements on active connections.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void hrpsSetupToSend(void)
{
  hrpsConn_t    *pConn = hrpsCb.conn;
  uint8_t       i;
  
  for (i = 0; i < DM_CONN_MAX; i++, pConn++)
  {
    if (pConn->connId != DM_CONN_ID_NONE)
    {      
      pConn->hrmToSend = TRUE;
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     hrpsFindNextToSend
 *        
 *  \brief  Find next connection with measurement to send.
 *
 *  \param  cccIdx  Heart rate measurement CCC descriptor index. 
 *
 *  \return Connection control block.
 */
/*************************************************************************************************/
static hrpsConn_t *hrpsFindNextToSend(uint8_t cccIdx)
{
  hrpsConn_t    *pConn = hrpsCb.conn;
  uint8_t       i;
  
  for (i = 0; i < DM_CONN_MAX; i++, pConn++)
  {
    if (pConn->connId != DM_CONN_ID_NONE && pConn->hrmToSend)
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
 *  \fn     hrpsBuildHrm
 *        
 *  \brief  Build a heart rate measurement characteristic.
 *
 *  \param  pBuf     Pointer to buffer to hold the built heart rate measurement characteristic.
 *  \param  pHrm     Heart rate measurement values.
 *
 *  \return Length of pBuf in bytes.
 */
/*************************************************************************************************/
static uint8_t hrpsBuildHrm(uint8_t *pBuf, appHrm_t *pHrm)
{
  uint8_t   *p = pBuf;
  uint8_t   flags = pHrm->flags;
  uint8_t   i;
  uint16_t  *pInterval;
  
  /* flags */
  UINT8_TO_BSTREAM(p, flags);
  
  /* heart rate measurement */
  if (flags & CH_HRM_FLAGS_VALUE_16BIT)
  {
    UINT16_TO_BSTREAM(p, (uint16_t) pHrm->heartRate);
  }
  else
  {
    UINT8_TO_BSTREAM(p, pHrm->heartRate);
  }
  
  /* energy expended */
  if (flags & CH_HRM_FLAGS_ENERGY_EXP)
  {
    UINT16_TO_BSTREAM(p, pHrm->energyExp);
  }
  
  /* rr interval */
  if (flags & CH_HRM_FLAGS_RR_INTERVAL)
  {
    pInterval = pHrm->pRrInterval;
    for (i = pHrm->numIntervals; i > 0; i--, pInterval++)
    {
      UINT16_TO_BSTREAM(p, *pInterval);
    }
  }

  /* return length */
  return (uint8_t) (p - pBuf);
}

/*************************************************************************************************/
/*!
 *  \fn     hrpsSendHrmNtf
 *        
 *  \brief  Send heart rate measurement notification
 *
 *  \return None.
 */
/*************************************************************************************************/
static void hrpsSendHrmNtf(dmConnId_t connId)
{
  uint8_t buf[ATT_DEFAULT_PAYLOAD_LEN];
  uint8_t len;
  
  /* build heart rate measurement characteristic */
  len = hrpsBuildHrm(buf, &hrpsCb.hrm);

  /* send notification */
  AttsHandleValueNtf(connId, HRS_HRM_HDL, len, buf);
}

/*************************************************************************************************/
/*!
 *  \fn     hrpsConnOpen
 *        
 *  \brief  Handle connection open.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void hrpsConnOpen(dmEvt_t *pMsg)
{
  hrpsCb.txReady = TRUE;
}

/*************************************************************************************************/
/*!
 *  \fn     hrpsHandleValueCnf
 *        
 *  \brief  Handle a received ATT handle value confirm.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void hrpsHandleValueCnf(attEvt_t *pMsg)
{
  hrpsConn_t  *pConn;

  if (pMsg->hdr.status == ATT_SUCCESS && pMsg->handle == HRS_HRM_HDL)
  {
    hrpsCb.txReady = TRUE;
  
    /* find next connection to send (note ccc idx is stored in timer status) */
    if ((pConn = hrpsFindNextToSend(hrpsCb.measTimer.msg.status)) != NULL)
    {
      hrpsSendHrmNtf(pConn->connId);
      hrpsCb.txReady = FALSE;
      pConn->hrmToSend = FALSE;
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     hrpsMeasTimerExp
 *        
 *  \brief  This function is called by the application when the periodic measurement
 *          timer expires.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void hrpsMeasTimerExp(wsfMsgHdr_t *pMsg)
{
  hrpsConn_t  *pConn;

  /* if there are active connections */
  if (hrpsNoConnActive() == FALSE)
  {
  
    /* set up heart rate measurement to be sent on all connections */
    hrpsSetupToSend();

    /* read heart rate measurement sensor data */
    AppHwHrmRead(&hrpsCb.hrm);
  
    /* if ready to send measurements */
    if (hrpsCb.txReady)
    {
      /* find next connection to send (note ccc idx is stored in timer status) */
      if ((pConn = hrpsFindNextToSend(pMsg->status)) != NULL)
      {
        hrpsSendHrmNtf(pConn->connId);
        hrpsCb.txReady = FALSE;
        pConn->hrmToSend = FALSE;
      }
    }
    
    /* restart timer */
    WsfTimerStartMs(&hrpsCb.measTimer, hrpsCb.cfg.period);
    
    /* increment energy expended for test/demonstration purposes */
    hrpsCb.hrm.energyExp++;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     HrpsInit
 *        
 *  \brief  Initialize the Heart Rate profile sensor.
 *
 *  \param  handerId    WSF handler ID of the application using this service.
 *  \param  pCfg        Configurable parameters.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HrpsInit(wsfHandlerId_t handlerId, hrpsCfg_t *pCfg)
{
  hrpsCb.measTimer.handlerId = handlerId;
  hrpsCb.cfg = *pCfg;
}

/*************************************************************************************************/
/*!
 *  \fn     HrpsMeasStart
 *        
 *  \brief  Start periodic heart rate measurement.  This function starts a timer to perform
 *          periodic measurements.
 *
 *  \param  connId      DM connection identifier.
 *  \param  timerEvt    WSF event designated by the application for the timer.
 *  \param  hrmCccIdx   Index of heart rate CCC descriptor in CCC descriptor handle table.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HrpsMeasStart(dmConnId_t connId, uint8_t timerEvt, uint8_t hrmCccIdx)
{
  /* if this is first connection */
  if (hrpsNoConnActive())
  {
    /* initialize control block */
    hrpsCb.measTimer.msg.event = timerEvt;
    hrpsCb.measTimer.msg.status = hrmCccIdx;  
    
    /* start timer */
    WsfTimerStartMs(&hrpsCb.measTimer, hrpsCb.cfg.period);
  }
  
  /* set conn id */
  hrpsCb.conn[connId - 1].connId = connId;
}

/*************************************************************************************************/
/*!
 *  \fn     HrpsMeasStop
 *        
 *  \brief  Stop periodic heart rate measurement.
 *
 *  \param  connId      DM connection identifier.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HrpsMeasStop(dmConnId_t connId)
{
  /* clear connection */
  hrpsCb.conn[connId - 1].connId = DM_CONN_ID_NONE;
  hrpsCb.conn[connId - 1].hrmToSend = FALSE;
  
  /* if no remaining connections */
  if (hrpsNoConnActive())
  {  
    /* stop timer */
    WsfTimerStop(&hrpsCb.measTimer);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     HrpsProcMsg
 *        
 *  \brief  Process received WSF message.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HrpsProcMsg(wsfMsgHdr_t *pMsg)
{
  if (pMsg->event == DM_CONN_OPEN_IND)
  {
    hrpsConnOpen((dmEvt_t *) pMsg);
  }
  else if (pMsg->event == ATTS_HANDLE_VALUE_CNF)
  {
    hrpsHandleValueCnf((attEvt_t *) pMsg);
  }
  else if (pMsg->event == hrpsCb.measTimer.msg.event)
  {
    hrpsMeasTimerExp(pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     HrpsWriteCback
 *        
 *  \brief  ATTS write callback for heart rate service.  Use this function as a parameter
 *          to SvcHrsCbackRegister().
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t HrpsWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                       uint16_t offset, uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{ 
  if (*pValue == CH_HRCP_RESET_ENERGY_EXP)
  {
    /* reset energy expended */
    hrpsCb.hrm.energyExp = 0;
    return ATT_SUCCESS;
  }
  else
  {
    /* else unknown control point command */
    return HRS_ERR_CP_NOT_SUP;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     HrpsSetFlags
 *        
 *  \brief  Set the heart rate measurement flags.
 *
 *  \param  flags      Heart rate measurement flags.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HrpsSetFlags(uint8_t flags)
{
  hrpsCb.hrm.flags = flags;
}
