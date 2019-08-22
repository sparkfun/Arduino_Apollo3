/*************************************************************************************************/
/*!
 *  \file   cfg_stack.h
 *
 *  \brief  Stack configuration.
 *
 *          $Date: 2017-03-10 14:08:37 -0600 (Fri, 10 Mar 2017) $
 *          $Revision: 11501 $
 *
 *  Copyright (c) 2009-2017 ARM Ltd., all rights reserved.
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
#ifndef CFG_STACK_H
#define CFG_STACK_H


#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  HCI
**************************************************************************************************/

/*! Vendor specific targets */
#define HCI_VS_GENERIC        0
#define HCI_VS_EMM            1

/*! Vendor specific target configuration */
#ifndef HCI_VS_TARGET
#define HCI_VS_TARGET HCI_VS_GENERIC
#endif

/* Extra byte allocation required for LL operations (i.e. MIC) in single-chip implementation */
#ifndef HCI_TX_DATA_TAILROOM
#define HCI_TX_DATA_TAILROOM  0
#endif

/**************************************************************************************************
  DM
**************************************************************************************************/

#undef  DM_CONN_MAX
/*! Maximum number of connections */
#define DM_CONN_MAX           8

/*! Number of supported advertising sets: must be set to 1 for legacy advertising */
#ifndef DM_NUM_ADV_SETS
#define DM_NUM_ADV_SETS       1
#endif

/*! Number of scanner and initiator PHYs (LE 1M, LE 2M and LE Coded): must be set to 1 for
    legacy scanner and initiator */
#ifndef DM_NUM_PHYS
#define DM_NUM_PHYS           1
#endif

/**************************************************************************************************
  L2C
**************************************************************************************************/

/*! Maximum number of connection oriented channels */
#ifndef L2C_COC_CHAN_MAX
#define L2C_COC_CHAN_MAX      8
#endif

/*! Maximum number of connection oriented channel registered clients */
#ifndef L2C_COC_REG_MAX
#define L2C_COC_REG_MAX       4
#endif

/**************************************************************************************************
  ATT
**************************************************************************************************/

/*! Maximum number of simultanous ATT notifications */
#ifndef ATT_NUM_SIMUL_NTF
#define ATT_NUM_SIMUL_NTF     10
#endif

/**************************************************************************************************
  SMP
**************************************************************************************************/


#ifdef __cplusplus
};
#endif

#endif /* CFG_STACK_H */
