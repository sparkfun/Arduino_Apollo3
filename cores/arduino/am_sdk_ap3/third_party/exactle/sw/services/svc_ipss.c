/*************************************************************************************************/
/*!
 *  \file   svc_ipss.h
 *
 *  \brief  Example Internet Profile Support Service implementation.
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
#include "att_uuid.h"
#include "wsf_trace.h"
#include "bstream.h"
#include "svc_ch.h"
#include "svc_ipss.h"
#include "svc_cfg.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef IPSS_SEC_PERMIT_READ
#define IPSS_SEC_PERMIT_READ (ATTS_PERMIT_READ | ATTS_PERMIT_READ_ENC)
#endif

/*! Characteristic write permissions */
#ifndef IPSS_SEC_PERMIT_WRITE
#define IPSS_SEC_PERMIT_WRITE  (ATTS_PERMIT_WRITE | ATTS_PERMIT_WRITE_ENC)
#endif

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/* IP Support service declaration */
static const uint8_t ipssValSvc[] = {UINT16_TO_BYTES(ATT_UUID_IP_SUPPORT_SERVICE)};
static const uint16_t ipssLenSvc = sizeof(ipssValSvc);

/* Attribute list for IPSS group */
static const attsAttr_t ipssList[] =
{
  /* IP Support Service declaration */
  {
    attPrimSvcUuid, 
    (uint8_t *) ipssValSvc,
    (uint16_t *) &ipssLenSvc, 
    sizeof(ipssValSvc),
    0,
    ATTS_PERMIT_READ
  },
};

/* IPSS group structure */
static attsGroup_t svcIpssGroup =
{
  NULL,
  (attsAttr_t *) ipssList,
  NULL,
  NULL,
  IPSS_START_HDL,
  IPSS_END_HDL
};

/*************************************************************************************************/
/*!
 *  \fn     SvcIpssAddGroup
 *        
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcIpssAddGroup(void)
{
  AttsAddGroup(&svcIpssGroup);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcIpssRemoveGroup
 *        
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcIpssRemoveGroup(void)
{
  AttsRemoveGroup(IPSS_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcIpssCbackRegister
 *        
 *  \brief  Register callbacks for the service.
 *
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcIpssCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
  svcIpssGroup.readCback = readCback;
  svcIpssGroup.writeCback = writeCback;
}
