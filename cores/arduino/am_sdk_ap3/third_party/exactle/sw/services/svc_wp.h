/*************************************************************************************************/
/*!
 *  \file   svc_wp.h
 *        
 *  \brief  ARM Ltd. proprietary service implementation.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *  
 *  Copyright (c) 2011-2017 ARM Ltd., all rights reserved.
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

#ifndef SVC_WP_H
#define SVC_WP_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/* Proprietary Service */
#define WP_START_HDL               0x200
#define WP_END_HDL                 (WP_MAX_HDL - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* Proprietary Service Handles */
enum
{
  WP_SVC_HDL = WP_START_HDL,       /* Proprietary service declaration */
  WP_DAT_CH_HDL,                   /* Proprietary data characteristic */ 
  WP_DAT_HDL,                      /* Proprietary data */
  WP_DAT_CH_CCC_HDL,               /* Proprietary data client characteristic configuration */
  WP_MAX_HDL
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcWpAddGroup(void);
void SvcWpRemoveGroup(void);
void SvcWpCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

#ifdef __cplusplus
};
#endif

#endif /* SVC_WP_H */
