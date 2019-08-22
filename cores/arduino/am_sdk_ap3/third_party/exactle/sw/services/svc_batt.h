/*************************************************************************************************/
/*!
 *  \file   svc_batt.h
 *        
 *  \brief  Example Battery service implementation.
 *
 *          $Date: 2017-02-08 14:15:55 -0600 (Wed, 08 Feb 2017) $
 *          $Revision: 11118 $
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

#ifndef SVC_BATT_H
#define SVC_BATT_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/* Battery Service */
#define BATT_START_HDL                    0x60
#define BATT_END_HDL                      (BATT_MAX_HDL - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* Battery Service Handles */
enum
{
  BATT_SVC_HDL = BATT_START_HDL,        /* Battery service declaration */
  BATT_LVL_CH_HDL,                      /* Battery level characteristic */
  BATT_LVL_HDL,                         /* Battery level */
  BATT_LVL_CH_CCC_HDL,                  /* Battery level CCCD */
  BATT_MAX_HDL
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcBattAddGroup(void);
void SvcBattRemoveGroup(void);
void SvcBattCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

/*************************************************************************************************/
/*!
 *  \fn     SvcBattAddGroupDyn
 *
 *  \brief  Add the battery service using the dynamic attribute subsystem.
 *
 *  \return None.
 */
/*************************************************************************************************/
void *SvcBattAddGroupDyn(void);

#ifdef __cplusplus
};
#endif

#endif /* SVC_BATT_H */

