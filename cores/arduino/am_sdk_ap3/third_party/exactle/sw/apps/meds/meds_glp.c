/*************************************************************************************************/
/*!
 *  \file   meds_glp.c
 *
 *  \brief  Medical sensor sample, glucose profile
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
#include "svc_gls.h"
#include "svc_dis.h"
#include "svc_core.h"
#include "glps_api.h"
#include "meds_main.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! enumeration of client characteristic configuration descriptors */
enum
{
  MEDS_GLS_GATT_SC_CCC_IDX,               /*! GATT service, service changed characteristic */
  MEDS_GLS_GLS_GLM_CCC_IDX,               /*! Glucose measurement characteristic */
  MEDS_GLS_GLS_GLMC_CCC_IDX,              /*! Glucose measurement context characteristic */
  MEDS_GLS_GLS_RACP_CCC_IDX,              /*! Glucose record access control point characteristic */
  MEDS_GLS_NUM_CCC_IDX
};

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/*! Service UUID list */
static const uint8_t medsSvcUuidList[] =
{
  UINT16_TO_BYTES(ATT_UUID_GLUCOSE_SERVICE),
  UINT16_TO_BYTES(ATT_UUID_DEVICE_INFO_SERVICE)  
};

/**************************************************************************************************
  Client Characteristic Configuration Descriptors
**************************************************************************************************/

/*! client characteristic configuration descriptors settings, indexed by above enumeration */
static const attsCccSet_t medsGlpCccSet[MEDS_GLS_NUM_CCC_IDX] =
{
  /* cccd handle          value range               security level */
  {GATT_SC_CH_CCC_HDL,    ATT_CLIENT_CFG_INDICATE,  DM_SEC_LEVEL_NONE},    /* MEDS_GLS_GATT_SC_CCC_IDX */
  {GLS_GLM_CH_CCC_HDL,    ATT_CLIENT_CFG_NOTIFY,    DM_SEC_LEVEL_NONE},    /* MEDS_GLS_GLS_GLM_CCC_IDX */
  {GLS_GLMC_CH_CCC_HDL,   ATT_CLIENT_CFG_NOTIFY,    DM_SEC_LEVEL_NONE},    /* MEDS_GLS_GLS_GLMC_CCC_IDX */
  {GLS_RACP_CH_CCC_HDL,   ATT_CLIENT_CFG_INDICATE,  DM_SEC_LEVEL_NONE}     /* MEDS_GLS_GLS_RACP_CCC_IDX */
};

/**************************************************************************************************
  Local Functions
**************************************************************************************************/

static void medsGlpStart(void);
static void medsGlpProcMsg(wsfMsgHdr_t *pMsg);
static void medsGlpBtn(dmConnId_t connId, uint8_t btn);

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! profile interface pointer */
medsIf_t medsGlpIf =
{
  medsGlpStart,
  medsGlpProcMsg,
  medsGlpBtn
};

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     medsGlpStart
 *        
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void medsGlpStart(void)
{
  /* set up CCCD table and callback */
  AttsCccRegister(MEDS_GLS_NUM_CCC_IDX, (attsCccSet_t *) medsGlpCccSet, medsCccCback);

  /* add glucose service */
  SvcGlsAddGroup();
  SvcGlsCbackRegister(NULL, GlpsRacpWriteCback);

  /* initialize glucose profile sensor */
  GlpsInit();
  GlpsSetCccIdx(MEDS_GLS_GLS_GLM_CCC_IDX, MEDS_GLS_GLS_GLMC_CCC_IDX, MEDS_GLS_GLS_RACP_CCC_IDX);

  /* TODO: Define glucose features */
  GlpsSetFeature(GLP_ALL_SUPPORTED_FEATURES);

  /* set advertising data */
  AppAdvSetAdValue(APP_ADV_DATA_DISCOVERABLE, DM_ADV_TYPE_16_UUID, sizeof(medsSvcUuidList),
                   (uint8_t *) medsSvcUuidList);
}

/*************************************************************************************************/
/*!
 *  \fn     medsGlpProcMsg
 *        
 *  \brief  Process messages from the event handler.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void medsGlpProcMsg(wsfMsgHdr_t *pMsg)
{
  GlpsProcMsg(pMsg);
}

/*************************************************************************************************/
/*!
 *  \fn     medsGlpBtn
 *        
 *  \brief  Button press callback.
 *
 *  \param  connId  Connection identifier.
 *  \param  btn     Button press.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void medsGlpBtn(dmConnId_t connId, uint8_t btn)
{
  GlpsBtn(connId, btn);
}
