/*************************************************************************************************/
/*!
 *  \file   svc_wdxs.c
 *
 *  \brief  Wireless Data Exchange service implementation.
 *
 *          $Date: 2017-03-09 12:18:38 -0600 (Thu, 09 Mar 2017) $
 *          $Revision: 11460 $
 *
 *  Copyright (c) 2013-2017 ARM Ltd., all rights reserved.
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

#include "wsf_types.h"
#include "svc_wdxs.h"
#include "att_api.h"
#include "wsf_trace.h"
#include "bstream.h"
#include "svc_ch.h"
#include "svc_cfg.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef WDXS_SEC_PERMIT_READ
#define WDXS_SEC_PERMIT_READ ATTS_PERMIT_READ
#endif

/*! Characteristic write permissions */
#ifndef WDXS_SEC_PERMIT_WRITE
#define WDXS_SEC_PERMIT_WRITE ATTS_PERMIT_WRITE
#endif

/**************************************************************************************************
 Global Variables
**************************************************************************************************/

/* UUIDs */
const uint8_t wdxsDcUuid[ATT_128_UUID_LEN] = {WDXS_DC_UUID};     /* WDX Device Configuration Characteristic */
const uint8_t wdxsFtcUuid[ATT_128_UUID_LEN] = {WDXS_FTC_UUID};   /* WDX File Transfer Control Characteristic */
const uint8_t wdxsFtdUuid[ATT_128_UUID_LEN] = {WDXS_FTD_UUID};   /* WDX File Transfer Data Characteristic */
const uint8_t wdxsAuUuid[ATT_128_UUID_LEN] = {WDXS_AU_UUID};     /* WDX Authentication Characteristic */

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/* Proprietary Service Declaration */
static const uint8_t wdxsValSvc[] = {UINT16_TO_BYTES(WDXS_SVC_UUID)};
static const uint16_t wdxsLenSvc = sizeof(wdxsValSvc);

/* WDX Device Configuration Characteristic Declaration */
static const uint8_t wdxsValDcCh[] = {ATT_PROP_NOTIFY | ATT_PROP_WRITE, UINT16_TO_BYTES(WDXS_DC_HDL), WDXS_DC_UUID};
static const uint16_t wdxsLenDcCh = sizeof(wdxsValDcCh);

/* WDX Device Configuration Characteristic Value */
/* Note these are dummy values */
static const uint8_t wdxsValDc[] = {0};
static const uint16_t wdxsLenDc = sizeof(wdxsValDc);

/* WDX Device Configuration CCCD */
static uint8_t wdxsValDcChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t wdxsLenDcChCcc = sizeof(wdxsValDcChCcc);

/* WDX File Transfer Control Characteristic Declaration */
static const uint8_t wdxsValFtcCh[] = {ATT_PROP_NOTIFY | ATT_PROP_WRITE_NO_RSP, UINT16_TO_BYTES(WDXS_FTC_HDL), WDXS_FTC_UUID};
static const uint16_t wdxsLenFtcCh = sizeof(wdxsValFtcCh);

/* WDX File Transfer Control Characteristic Value */
/* Note these are dummy values */
static const uint8_t wdxsValFtc[] = {0};
static const uint16_t wdxsLenFtc = sizeof(wdxsValFtc);

/* WDX File Transfer Control CCCD */
static uint8_t wdxsValFtcChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t wdxsLenFtcChCcc = sizeof(wdxsValFtcChCcc);

/* WDX File Transfer Data Characteristic Declaration */
static const uint8_t wdxsValFtdCh[] = {ATT_PROP_NOTIFY | ATT_PROP_WRITE_NO_RSP, UINT16_TO_BYTES(WDXS_FTD_HDL), WDXS_FTD_UUID};
static const uint16_t wdxsLenFtdCh = sizeof(wdxsValFtdCh);

/* WDX File Transfer Data Characteristic Value */
/* Note these are dummy values */
static const uint8_t wdxsValFtd[] = {0};
static const uint16_t wdxsLenFtd = sizeof(wdxsValFtd);

/* WDX File Transfer Data CCCD */
static uint8_t wdxsValFtdChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t wdxsLenFtdChCcc = sizeof(wdxsValFtdChCcc);

/* WDX Authentication Characteristic Declaration */
static const uint8_t wdxsValAuCh[] = {ATT_PROP_NOTIFY | ATT_PROP_WRITE, UINT16_TO_BYTES(WDXS_AU_HDL), WDXS_AU_UUID};
static const uint16_t wdxsLenAuCh = sizeof(wdxsValAuCh);

/* WDX Authentication Characteristic Value */
/* Note these are dummy values */
static const uint8_t wdxsValAu[] = {0};
static const uint16_t wdxsLenAu = sizeof(wdxsValAu);

/* WDX Authentication CCCD */
static uint8_t wdxsValAuChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t wdxsLenAuChCcc = sizeof(wdxsValAuChCcc);

/* Attribute list for WDX group */
static const attsAttr_t wdxsList[] =
{
  /* Service Delcaration */
  {
    attPrimSvcUuid,
    (uint8_t *) wdxsValSvc,
    (uint16_t *) &wdxsLenSvc,
    sizeof(wdxsValSvc),
    0,
    ATTS_PERMIT_READ
  },
  /* WDX Device Configuration Characteristic Declaration */
  {
    attChUuid,
    (uint8_t *) wdxsValDcCh,
    (uint16_t *) &wdxsLenDcCh,
    sizeof(wdxsValDcCh),
    0,
    ATTS_PERMIT_READ
  },
  /* WDX Device Configuration Characteristic Value */
  {
    wdxsDcUuid,
    (uint8_t *) wdxsValDc,
    (uint16_t *) &wdxsLenDc,
    ATT_DEFAULT_PAYLOAD_LEN,
    (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
    WDXS_SEC_PERMIT_WRITE
  },
  /* WDX Device Configuration CCCD */
  {
    attCliChCfgUuid,
    (uint8_t *) wdxsValDcChCcc,
    (uint16_t *) &wdxsLenDcChCcc,
    sizeof(wdxsValDcChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | WDXS_SEC_PERMIT_WRITE)
  },
  /* WDX File Transfer Control Characteristic Declaration */
  {
    attChUuid,
    (uint8_t *) wdxsValFtcCh,
    (uint16_t *) &wdxsLenFtcCh,
    sizeof(wdxsValFtcCh),
    0,
    ATTS_PERMIT_READ
  },
  /* WDX File Transfer Control Characteristic Value */
  {
    wdxsFtcUuid,
    (uint8_t *) wdxsValFtc,
    (uint16_t *) &wdxsLenFtc,
    ATT_DEFAULT_PAYLOAD_LEN,
    (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
    WDXS_SEC_PERMIT_WRITE
  },
  /* WDX File Transfer Control CCCD */
  {
    attCliChCfgUuid,
    (uint8_t *) wdxsValFtcChCcc,
    (uint16_t *) &wdxsLenFtcChCcc,
    sizeof(wdxsValFtcChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | WDXS_SEC_PERMIT_WRITE)
  },
  /* WDX File Transfer Data Characteristic Declaration */
  {
    attChUuid,
    (uint8_t *) wdxsValFtdCh,
    (uint16_t *) &wdxsLenFtdCh,
    sizeof(wdxsValFtdCh),
    0,
    ATTS_PERMIT_READ
  },
  /* WDX File Transfer Data Characteristic Value */
  {
    wdxsFtdUuid,
    (uint8_t *) wdxsValFtd,
    (uint16_t *) &wdxsLenFtd,
    ATT_DEFAULT_PAYLOAD_LEN,
    (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
    WDXS_SEC_PERMIT_WRITE
  },
  /* WDX File Transfer Data CCCD */
  {
    attCliChCfgUuid,
    (uint8_t *) wdxsValFtdChCcc,
    (uint16_t *) &wdxsLenFtdChCcc,
    sizeof(wdxsValFtdChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | WDXS_SEC_PERMIT_WRITE)
  },
  /* WDX Authentication Characteristic Declaration */
  {
    attChUuid,
    (uint8_t *) wdxsValAuCh,
    (uint16_t *) &wdxsLenAuCh,
    sizeof(wdxsValAuCh),
    0,
    ATTS_PERMIT_READ
  },
  /* WDX Authentication Characteristic Value */
  {
    wdxsAuUuid,
    (uint8_t *) wdxsValAu,
    (uint16_t *) &wdxsLenAu,
    ATT_DEFAULT_PAYLOAD_LEN,
    (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
    WDXS_SEC_PERMIT_WRITE
  },
  /* WDX Authentication CCCD */
  {
    attCliChCfgUuid,
    (uint8_t *) wdxsValAuChCcc,
    (uint16_t *) &wdxsLenAuChCcc,
    sizeof(wdxsValAuChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | WDXS_SEC_PERMIT_WRITE)
  }
};

/* WDX group structure */
static attsGroup_t svcWdxsGroup =
{
  NULL,
  (attsAttr_t *) wdxsList,
  NULL,
  NULL,
  WDXS_START_HDL,
  WDXS_END_HDL
};

/*************************************************************************************************/
/*!
 *  \fn     SvcWdxsAddGroup
 *
 *  \brief  Add the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcWdxsAddGroup(void)
{
  AttsAddGroup(&svcWdxsGroup);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcWdxsRemoveGroup
 *
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcWdxsRemoveGroup(void)
{
  AttsRemoveGroup(WDXS_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcWdxsRegister
 *
 *  \brief  Register a write callback functions for the ATT Group.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcWdxsRegister(attsWriteCback_t writeCb)
{
  svcWdxsGroup.writeCback = writeCb;
}
