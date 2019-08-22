/*************************************************************************************************/
/*!
 *  \file   app_cfg.h
 *
 *  \brief  Application framework configuration.
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
#ifndef APP_CFG_H
#define APP_CFG_H


#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Number of application database device records */
#ifndef APP_DB_NUM_RECS
#define APP_DB_NUM_RECS 3
#endif

/*! Number of client characteristic configuration descriptor handles per record */
#ifndef APP_DB_NUM_CCCD
#define APP_DB_NUM_CCCD 10
#endif

/*! Number of ATT client cached handles per record */
#ifndef APP_DB_HDL_LIST_LEN
#define APP_DB_HDL_LIST_LEN 20
#endif

/*! Number of scan results to store (used only when operating as master) */
#ifndef APP_SCAN_RESULT_MAX
#define APP_SCAN_RESULT_MAX 10
#endif

#ifdef __cplusplus
};
#endif

#endif /* APP_CFG_H */
