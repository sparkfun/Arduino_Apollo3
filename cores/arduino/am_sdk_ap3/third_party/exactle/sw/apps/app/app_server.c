/*************************************************************************************************/
/*!
 *  \file   app_server.c
 *
 *  \brief  Application framework module for attribute server.
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

#include "wsf_types.h"
#include "wsf_msg.h"
#include "dm_api.h"
#include "att_api.h"
#include "app_api.h"
#include "app_main.h"

/*************************************************************************************************/
/*!
*  \fn     appServerSetSigningInfo
*
*  \brief  Set the peer's CSRK and sign counter on this connection.
*
*  \param  connId      DM connection ID.
*
*  \return None.
*/
/*************************************************************************************************/
static void appServerSetSigningInfo(dmConnId_t connId)
{
  appDbHdl_t  dbHdl;
  dmSecKey_t  *pPeerKey;

  /* if peer's CSRK is available */
  if (((dbHdl = AppDbGetHdl(connId)) != APP_DB_HDL_NONE) &&
      ((pPeerKey = AppDbGetKey(dbHdl, DM_KEY_CSRK, NULL)) != NULL))
  {
    /* set peer's CSRK and sign counter on this connection */
    AttsSetCsrk(connId, pPeerKey->csrk.key);
    AttsSetSignCounter(connId, AppDbGetPeerSignCounter(dbHdl));
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppServerConnCback
 *        
 *  \brief  ATT connection callback for app framework.
 *
 *  \param  pDmEvt  DM callback event.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppServerConnCback(dmEvt_t *pDmEvt)
{
  bool_t      bonded;
  appDbHdl_t  dbHdl;
  dmConnId_t  connId = (dmConnId_t) pDmEvt->hdr.param;
  
  if (pDmEvt->hdr.event == DM_CONN_OPEN_IND)
  {
    /* set up CCC table with uninitialized (all zero) values */
    AttsCccInitTable(connId, NULL);

    /* set peer's data signing info */
    appServerSetSigningInfo(connId);
  }
  else if (pDmEvt->hdr.event == DM_SEC_PAIR_CMPL_IND)
  {
    bonded = ((pDmEvt->pairCmpl.auth & DM_AUTH_BOND_FLAG) == DM_AUTH_BOND_FLAG);
    
    /* if going from unbonded to bonded update CCC table */
    if (bonded && (appCheckBonded(connId) == FALSE))
    {
      if ((dbHdl = AppDbGetHdl(connId)) != APP_DB_HDL_NONE)
      {
        AttsCccInitTable(connId, AppDbGetCccTbl(dbHdl));
      }
    }      

    /* set peer's data signing info */
    appServerSetSigningInfo(connId);
  }
  else if (pDmEvt->hdr.event == DM_SEC_ENCRYPT_IND)
  {
    /* if going from unbonded to bonded update CCC table */
    if (pDmEvt->encryptInd.usingLtk && appCheckBondByLtk(connId))
    {
      if ((dbHdl = AppDbGetHdl(connId)) != APP_DB_HDL_NONE)
      {
        AttsCccInitTable(connId, AppDbGetCccTbl(dbHdl));
      }
    }    
  }
  else if (pDmEvt->hdr.event == DM_CONN_CLOSE_IND)
  {
    /* clear CCC table on connection close */
    AttsCccClearTable(connId);

    if ((dbHdl = AppDbGetHdl(connId)) != APP_DB_HDL_NONE)
    {
      /* remember peer's sign counter */
      AppDbSetPeerSignCounter(dbHdl, AttsGetSignCounter(connId));
    }
  }
}
