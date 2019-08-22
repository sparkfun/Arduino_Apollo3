/*************************************************************************************************/
/*!
 *  \file   svc_scpss.c
 *        
 *  \brief  Example Scan Parameter Service Server implementation.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
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

#include "wsf_types.h"
#include "att_api.h"
#include "wsf_trace.h"
#include "bstream.h"
#include "svc_ch.h"
#include "svc_scpss.h"
#include "svc_cfg.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef SCPSS_SEC_PERMIT_READ
#define SCPSS_SEC_PERMIT_READ (ATTS_PERMIT_READ | ATTS_PERMIT_READ_ENC)
#endif

/*! Characteristic write permissions */
#ifndef SCPSS_SEC_PERMIT_WRITE
#define SCPSS_SEC_PERMIT_WRITE  (ATTS_PERMIT_WRITE | ATTS_PERMIT_WRITE_ENC)
#endif

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/*Scan parameter service declaration */
static const uint8_t scpssValSvc[] = {UINT16_TO_BYTES(ATT_UUID_SCAN_PARAM_SERVICE)};
static const uint16_t scpssLenSvc = sizeof(scpssValSvc);

/* Scan interval window characteristic */ 
static const uint8_t scpssValSiwCh[] = {ATT_PROP_WRITE_NO_RSP, UINT16_TO_BYTES(SCPSS_SIW_HDL), UINT16_TO_BYTES(ATT_UUID_SCAN_INT_WIND)};
static const uint16_t scpssLenSiwCh = sizeof(scpssValSiwCh);

/* Scan interval window measurement */
/* Note these are dummy values */
static const uint8_t scpssValSiw[] = {0};
static const uint16_t scpssLenSiw = CH_SCPP_INTERVAL_WINDOW_LEN;


/* Attribute list for SCPSS group */
static const attsAttr_t scpssList[] =
{
  /* Scan Parameter Service declaration */
  {
    attPrimSvcUuid, 
    (uint8_t *) scpssValSvc,
    (uint16_t *) &scpssLenSvc, 
    sizeof(scpssValSvc),
    0,
    ATTS_PERMIT_READ
  },
  /* Scan interval window characteristic */ 
  {
    attChUuid,
    (uint8_t *)scpssValSiwCh,
    (uint16_t *) &scpssLenSiwCh,
    sizeof(scpssValSiwCh),
    0,
    ATTS_PERMIT_READ
  },
  /* Scan interval window */
  {
    attSiwChUuid,
    (uint8_t *)scpssValSiw,
    (uint16_t *) &scpssLenSiw,
    CH_SCPP_INTERVAL_WINDOW_LEN,
    ATTS_SET_WRITE_CBACK,
    SCPSS_SEC_PERMIT_WRITE
  },
};

/* SCPSS group structure */
static attsGroup_t svcScpssGroup =
{
  NULL,
  (attsAttr_t *) scpssList,
  NULL,
  NULL,
  SCPSS_START_HDL,
  SCPSS_END_HDL
};

/*************************************************************************************************/
/*!
 *  \fn     SvcScpssAddGroup
 *        
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcScpssAddGroup(void)
{
  AttsAddGroup(&svcScpssGroup);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcScpssRemoveGroup
 *        
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcScpssRemoveGroup(void)
{
  AttsRemoveGroup(SCPSS_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcScpssCbackRegister
 *        
 *  \brief  Register callbacks for the service.
 *
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcScpssCbackRegister(attsWriteCback_t writeCback)
{
  svcScpssGroup.writeCback = writeCback;
}
