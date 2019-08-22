/*************************************************************************************************/
/*!
 *  \file   svc_hrs.h
 *        
 *  \brief  Example Heart Rate service implementation.
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

#ifndef SVC_HRS_H
#define SVC_HRS_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/* Error Codes */
#define HRS_ERR_CP_NOT_SUP          0x80    /* Control Point value not supported */

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/* Heart Rate Service */
#define HRS_START_HDL               0x20
#define HRS_END_HDL                 (HRS_MAX_HDL - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* Heart Rate Service Handles */
enum
{
  HRS_SVC_HDL = HRS_START_HDL,      /* Heart rate service declaration */
  HRS_HRM_CH_HDL,                   /* Heart rate measurement characteristic */ 
  HRS_HRM_HDL,                      /* Heart rate measurement */
  HRS_HRM_CH_CCC_HDL,               /* Heart rate measurement client characteristic configuration */
  HRS_SL_CH_HDL,                    /* Body sensor location characteristic */ 
  HRS_SL_HDL,                       /* Body sensor location */
  HRS_CP_CH_HDL,                    /* Heart rate control point characteristic */
  HRS_CP_HDL,                       /* Heart rate control point */
  HRS_MAX_HDL
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcHrsAddGroup(void);
void SvcHrsRemoveGroup(void);
void SvcHrsCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

#ifdef __cplusplus
};
#endif

#endif /* SVC_HRS_H */
