/*************************************************************************************************/
/*!
 *  \file   svc_cscs.h
 *        
 *  \brief  Example Cycling Speed and Cadence Service Server implementation.
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

#ifndef SVC_CSCS_H
#define SVC_CSCS_H

#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
Constants 
**************************************************************************************************/

/* Cycle Speed and Cadence Feature Bits of the Feature Characteristic */
#define CSCS_WRDS_FEATURE_BIT            (1<<0)  /* Wheel Revolution Data Supported Feature Mask Bit */
#define CSCS_CRDS_FEATURE_BIT            (1<<1)  /* Crank Revolution Data Supported Feature Mask Bit */
#define CSCS_MSLS_FEATURE_BIT            (1<<2)  /* Multiple Sensor Locations Supporte Feature Mask Bit */

/* TODO: Set to all supported features */
#define CSCS_ALL_FEATURES                (0x3)   /* All Supported Feature Mask */

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/* Cycling Speed Service */
#define CSCS_START_HDL               0x0450
#define CSCS_END_HDL                 (CSCS_MAX_HDL - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* Cycling Speed Service Handles */
enum
{
  CSCS_SVC_HDL = CSCS_START_HDL,       /* Cycling Speed Server Service declaration */
  CSCS_CSF_CH_HDL,                     /* Cycling Speed Feature characteristic */
  CSCS_CSF_HDL,                        /* Cycling Speed Feature */
  CSCS_CSM_CH_HDL,                     /* Cycling Speed Measurement characteristic */
  CSCS_CSM_HDL,                        /* Cycling Speed Measurement */
  CSCS_CSM_CH_CCC_HDL,                 /* Cycling Speed Measurement Client Characteristic Configuration Descriptor */
  CSCS_SL_CH_HDL,                      /* Cycling Speed Sensor Location characteristic */
  CSCS_SL_HDL,                         /* Cycling Speed Sensor Location */
  CSCS_MAX_HDL
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcCscsAddGroup(void);
void SvcCscsRemoveGroup(void);
void SvcCscsCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

#ifdef __cplusplus
};
#endif

#endif /* SVC_CSCS_H */
