/*************************************************************************************************/
/*!
 *  \file   scpps_main.c
 *
 *  \brief  Scan Parameter Profile Server.
 *
 *          $Date: 2012-05-17 12:55:46 -0700 (Thu, 17 May 2012) $
 *          $Revision: 324 $
 *
 *  Copyright (c) 2016 ARM Inc., all rights reserved.
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
#include "wsf_assert.h"
#include "wsf_trace.h"
#include "bstream.h"
#include "app_api.h"
#include "scpps_api.h"
#include "svc_scpss.h"

/**************************************************************************************************
  Local Variables
**************************************************************************************************/
static ScppsAppCback_t *scppsAppCback;

/*************************************************************************************************/
/*!
*  \fn     ScppsRegisterCback
*
*  \brief  Called to register an application callback function
*
*  \return Status
*/
/*************************************************************************************************/
void ScppsRegisterCback(ScppsAppCback_t *cback)
{
  scppsAppCback = cback;
}

/*************************************************************************************************/
/*!
 *  \fn     ScppsAttsWriteCback
 *        
 *  \brief  Called when the peer writes to SCPPS attributes
 *
 *  \return Status
 */
/*************************************************************************************************/
uint8_t ScppsAttsWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                            uint16_t offset, uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
  if (scppsAppCback && handle == SCPSS_SIW_HDL)
  {
    uint16_t interval;
    uint16_t window;

    BSTREAM_TO_UINT16(interval, pValue);
    BSTREAM_TO_UINT16(window, pValue);

    /* Call the callback to the application layer */
    (*scppsAppCback)(connId, interval, window);
  }

  return 0;
}
