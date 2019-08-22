/*************************************************************************************************/
/*!
 *  \file   gap_main.c
 *
 *  \brief  GAP profile.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2015-2017 ARM Ltd., all rights reserved.
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

#include "wsf_types.h"
#include "wsf_assert.h"
#include "wsf_trace.h"
#include "app_db.h"
#include "app_api.h"
#include "gap_api.h"

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! GAP service characteristics for discovery */

/*! Central Address Resolution */  
static const attcDiscChar_t gapCar = 
{
  attCarChUuid,
  0
};

/*! Resolvable Private Address Only */
static const attcDiscChar_t gapRpao =
{
  attRpaoChUuid,
  0
};

/*! List of characteristics to be discovered; order matches handle index enumeration  */
static const attcDiscChar_t *gapDiscCharList[] =
{
  &gapCar,                   /* Central Address Resolution */
  &gapRpao                   /* Resolvable Private Address Only */
};

/* sanity check:  make sure handle list length matches characteristic list length */
WSF_CT_ASSERT(GAP_HDL_LIST_LEN == ((sizeof(gapDiscCharList) / sizeof(attcDiscChar_t *))));

/*************************************************************************************************/
/*!
 *  \fn     GapDiscover
 *        
 *  \brief  Perform service and characteristic discovery for GAP service.  Note that pHdlList
 *          must point to an array of handles of length GAP_HDL_LIST_LEN.  If discovery is
 *          successful the handles of discovered characteristics and descriptors will be set
 *          in pHdlList.
 *
 *  \param  connId    Connection identifier.
 *  \param  pHdlList  Characteristic handle list.
 *
 *  \return None.
 */
/*************************************************************************************************/
void GapDiscover(dmConnId_t connId, uint16_t *pHdlList)
{
  AppDiscFindService(connId, ATT_16_UUID_LEN, (uint8_t *) attGapSvcUuid,
                     GAP_HDL_LIST_LEN, (attcDiscChar_t **) gapDiscCharList, pHdlList);
}

/*************************************************************************************************/
/*!
 *  \fn     GapValueUpdate
 *        
 *  \brief  Process a value received in an ATT read response, notification, or indication 
 *          message.  Parameter pHdlList must point to an array of length GAP_HDL_LIST_LEN. 
 *          If the attribute handle of the message matches a handle in the handle list the value
 *          is processed, otherwise it is ignored.
 *
 *  \param  pHdlList  Characteristic handle list.
 *  \param  pMsg      ATT callback message.
 *
 *  \return ATT_SUCCESS if handle is found, ATT_ERR_NOT_FOUND otherwise.
 */
/*************************************************************************************************/
uint8_t GapValueUpdate(uint16_t *pHdlList, attEvt_t *pMsg)
{
  uint8_t status = ATT_SUCCESS;
  
  /* service changed */
  if (pMsg->handle == pHdlList[GAP_CAR_HDL_IDX])
  {
    appDbHdl_t dbHdl;

    /* if there's a device record */
    if ((dbHdl = AppDbGetHdl((dmConnId_t)pMsg->hdr.param)) != APP_DB_HDL_NONE)
    {
      if ((pMsg->pValue[0] == FALSE) || (pMsg->pValue[0] == TRUE))
      {
        /* store value in device database */
        AppDbSetPeerAddrRes(dbHdl, pMsg->pValue[0]);
      }
      else
      {
        /* invalid value */
        status = ATT_ERR_RANGE;
      }

      APP_TRACE_INFO1("Central address resolution: %d", pMsg->pValue[0]);
    }
  }
  /* handle not found in list */
  else
  {
    status = ATT_ERR_NOT_FOUND;
  }
  
  return status;
}
