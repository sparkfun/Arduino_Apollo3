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

#ifndef SVC_IPSS_H
#define SVC_IPSS_H

#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/* IP Support Service */
#define IPSS_START_HDL               0x0600
#define IPSS_END_HDL                 (IPSS_MAX_HDL - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* IP Support Service Handles */
enum
{
  IPSS_SVC_HDL = IPSS_START_HDL,       /* IP Support Server Service declaration */
  IPSS_MAX_HDL
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcIpssAddGroup(void);
void SvcIpssRemoveGroup(void);
void SvcIpssCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

#ifdef __cplusplus
};
#endif

#endif /* SVC_IPSS_H */
