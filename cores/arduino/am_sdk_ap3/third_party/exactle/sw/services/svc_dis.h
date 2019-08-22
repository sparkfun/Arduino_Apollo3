/*************************************************************************************************/
/*!
 *  \file   svc_dis.h
 *        
 *  \brief  Example Device Information Service implementation.
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

#ifndef SVC_DIS_H
#define SVC_DIS_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/* Device Information Service */
#define DIS_START_HDL               0x30
#define DIS_END_HDL                 (DIS_MAX_HDL - 1)

/**************************************************************************************************
 Handles
**************************************************************************************************/

/* Device Information Service Handles */
enum
{
  DIS_SVC_HDL = DIS_START_HDL,      /* Information service declaration */
  DIS_MFR_CH_HDL,                   /* Manufacturer name string characteristic */
  DIS_MFR_HDL,                      /* Manufacturer name string */
  DIS_SID_CH_HDL,                   /* System ID characteristic */
  DIS_SID_HDL,                      /* System ID */
  DIS_MN_CH_HDL,                    /* Model number string characteristic */
  DIS_MN_HDL,                       /* Model number string */
  DIS_SN_CH_HDL,                    /* Serial number string characteristic */
  DIS_SN_HDL,                       /* Serial number string */
  DIS_FWR_CH_HDL,                   /* Firmware revision string characteristic */
  DIS_FWR_HDL,                      /* Firmware revision string */
  DIS_HWR_CH_HDL,                   /* Hardware revision string characteristic */
  DIS_HWR_HDL,                      /* Hardware revision string */
  DIS_SWR_CH_HDL,                   /* Software revision string characteristic */
  DIS_SWR_HDL,                      /* Software revision string */
  DIS_RCD_CH_HDL,                   /* Registration certificate data characteristic */
  DIS_RCD_HDL,                      /* Registration certificate data */
  DIS_MAX_HDL
};

/**************************************************************************************************
Macros
**************************************************************************************************/

/* Sizes of attributes */
#ifndef DIS_MAXSIZE_MFR_ATT
#define DIS_MAXSIZE_MFR_ATT         20  /* Size of manufacturer name string attribute */
#endif

#ifndef DIS_MAXSIZE_MN_ATT
#define DIS_MAXSIZE_MN_ATT          20  /* Size of model number string attribute */
#endif

#ifndef DIS_MAXSIZE_SN_ATT
#define DIS_MAXSIZE_SN_ATT          20  /* Size of serial number string attribute */
#endif

#ifndef DIS_MAXSIZE_FWR_ATT
#define DIS_MAXSIZE_FWR_ATT         16  /* Size of firmware revision string attribute */
#endif

#ifndef DIS_MAXSIZE_HWR_ATT
#define DIS_MAXSIZE_HWR_ATT         16  /* Size of hardware revision string attribute */
#endif

#ifndef DIS_MAXSIZE_SWR_ATT
#define DIS_MAXSIZE_SWR_ATT         16  /* Size of software revision string attribute */
#endif

#ifndef DIS_SIZE_SID_ATT
#define DIS_SIZE_SID_ATT            8   /* Size of system id attribute */
#endif

#ifndef DIS_SIZE_RCD_ATT
#define DIS_SIZE_RCD_ATT            6   /* Size of registration certificate data attribute */
#endif

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void SvcDisAddGroup(void);
void SvcDisRemoveGroup(void);

#ifdef __cplusplus
};
#endif

#endif /* SVC_DIS_H */

