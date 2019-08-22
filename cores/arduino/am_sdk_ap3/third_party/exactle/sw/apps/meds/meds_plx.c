/*************************************************************************************************/
/*!
 *  \file   meds_plx.c
 *
 *  \brief  Medical sensor sample, pulse oximeter profile
 *
 *          $Date: 2012-06-19 16:33:23 -0700 (Tue, 19 Jun 2012) $
 *          $Revision: 334 $
 *
 *  Copyright (c) 2016-2017 ARM Ltd., all rights reserved.
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
#include "bstream.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "hci_api.h"
#include "dm_api.h"
#include "att_api.h"
#include "app_api.h"
#include "app_ui.h"
#include "app_hw.h"
#include "svc_ch.h"
#include "svc_plxs.h"
#include "svc_dis.h"
#include "svc_core.h"
#include "plxps_api.h"
#include "plxps_main.h"
#include "meds_main.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! enumeration of client characteristic configuration descriptors */
enum
{
  MEDS_PLX_GATT_SC_CCC_IDX,               /*! GATT service, service changed characteristic */
  MEDS_PLX_PLX_SCM_CCC_IDX,               /*! Pulse Oximeter Spot Check measurement characteristic */
  MEDS_PLX_PLX_CM_CCC_IDX,                /*! Pulse Oximeter Continuous characteristic */
  MEDS_PLX_PLX_RACP_CCC_IDX,              /*! Pulse Oximeter record access control point characteristic */
  MEDS_PLX_NUM_CCC_IDX
};

/* Default MTU */
#define MEDS_PLX_DEFAULT_MTU              50

/**************************************************************************************************
  Configurable Parameters
**************************************************************************************************/

/*! Pulse Oximeter Continuous measurement configuration */
static const plxpsCfg_t medsPlxpsCfg =
{
  2000      /*! Measurement timer expiration period in ms */
};

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/*! Service UUID list */
static const uint8_t medsSvcUuidList[] =
{
  UINT16_TO_BYTES(ATT_UUID_PULSE_OXIMITER_SERVICE),
  UINT16_TO_BYTES(ATT_UUID_DEVICE_INFO_SERVICE)  
};

/**************************************************************************************************
  Client Characteristic Configuration Descriptors
**************************************************************************************************/

/*! client characteristic configuration descriptors settings, indexed by above enumeration */
static const attsCccSet_t medsPlxCccSet[MEDS_PLX_NUM_CCC_IDX] =
{
  /* cccd handle                  value range               security level */
  {GATT_SC_CH_CCC_HDL,            ATT_CLIENT_CFG_INDICATE,  DM_SEC_LEVEL_NONE},    /* MEDS_PLX_GATT_SC_CCC_IDX */
  {PLXS_SPOT_CHECK_CH_CCC_HDL,    ATT_CLIENT_CFG_INDICATE,  DM_SEC_LEVEL_NONE},    /* MEDS_PLX_PLX_SCM_CCC_IDX */
  {PLXS_CONTINUOUS_CH_CCC_HDL,    ATT_CLIENT_CFG_NOTIFY,    DM_SEC_LEVEL_NONE},    /* MEDS_PLX_PLX_CM_CCC_IDX */
  {PLXS_RECORD_ACCESS_CH_CCC_HDL, ATT_CLIENT_CFG_INDICATE,  DM_SEC_LEVEL_NONE}     /* MEDS_PLX_PLX_RACP_CCC_IDX */
};

/**************************************************************************************************
  Local Functions
**************************************************************************************************/

static void medsPlxStart(void);
static void medsPlxProcMsg(wsfMsgHdr_t *pMsg);
static void medsPlxBtn(dmConnId_t connId, uint8_t btn);

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! profile interface pointer */
medsIf_t medsPlxIf =
{
  medsPlxStart,
  medsPlxProcMsg,
  medsPlxBtn
};

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! application control block */
static struct
{
  bool_t            measuring;
} medsPlxCb;

/*************************************************************************************************/
/*!
 *  \fn     medsPlxClose
 *        
 *  \brief  Perform actions on connection close.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void medsPlxClose(wsfMsgHdr_t *pMsg)
{
  /* Reset control information */
  medsPlxCb.measuring = FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     medsPlxStart
 *        
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void medsPlxStart(void)
{
  /* set up CCCD table and callback */
  AttsCccRegister(MEDS_PLX_NUM_CCC_IDX, (attsCccSet_t *) medsPlxCccSet, medsCccCback);

  /* add pulse oximeter service */
  SvcPlxsAddGroup();
  SvcPlxsCbackRegister(NULL, PlxpsWriteCback);

  /* initialize pulse oximeter profile sensor */
  PlxpsInit(medsCb.handlerId, (plxpsCfg_t *) &medsPlxpsCfg);
  PlxpsSetCccIdx(MEDS_PLX_PLX_SCM_CCC_IDX, MEDS_PLX_PLX_CM_CCC_IDX, MEDS_PLX_PLX_RACP_CCC_IDX);

  /* TODO: Define pulse oximeter features */
  PlxpsSetFeature(CH_PLF_FLAG_SENSOR_STATUS_SUP, 0, 0);

  /* set advertising data */
  AppAdvSetAdValue(APP_ADV_DATA_DISCOVERABLE, DM_ADV_TYPE_16_UUID, sizeof(medsSvcUuidList),
                   (uint8_t *) medsSvcUuidList);
}

/*************************************************************************************************/
/*!
 *  \fn     medsPlxProcMsg
 *        
 *  \brief  Process messages from the event handler.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void medsPlxProcMsg(wsfMsgHdr_t *pMsg)
{
    if (pMsg->event == DM_CONN_OPEN_IND)
  {
    AttcMtuReq((dmConnId_t) pMsg->param, MEDS_PLX_DEFAULT_MTU);
  }
  else if (pMsg->event == DM_CONN_CLOSE_IND)
  {
    medsPlxClose(pMsg);
  }

  PlxpsProcMsg(pMsg);
}

/*************************************************************************************************/
/*!
 *  \fn     medsPlxBtn
 *        
 *  \brief  Button press callback.
 *
 *  \param  connId  Connection identifier.
 *  \param  btn     Button press.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void medsPlxBtn(dmConnId_t connId, uint8_t btn)
{
  appPlxCm_t record;

  /* button actions when connected */
  if (connId != DM_CONN_ID_NONE)
  {
    switch (btn)
    {
      case APP_UI_BTN_2_SHORT:
        AppHwPlxcmRead(&record);
        record.flags = CH_PLXC_FLAG_PULSE_AMP_INDX;
        plxpsSendContinuousMeas(connId, &record);
        break;
      
      case APP_UI_BTN_2_MED:
        plxpsDbDeleteRecords(CH_RACP_OPERATOR_ALL);
        break;

      case APP_UI_BTN_1_SHORT:
        /* start or complete measurement */
        if (!medsPlxCb.measuring)
        {
          PlxpsMeasStart(connId, MEDS_TIMER_IND, MEDS_PLX_PLX_CM_CCC_IDX);
          medsPlxCb.measuring = TRUE;
        }
        else
        {
          PlxpsMeasStop();
          medsPlxCb.measuring = FALSE;
        }
        break;
        
      default:
        break;
    }    
  }
}
