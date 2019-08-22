/*************************************************************************************************/
/*!
 *  \file   wdxs_au.c
 *
 *  \brief  Wireless Data Exchange profile implementation - Authentication.
 *
 *          $Date: 2017-03-09 12:18:38 -0600 (Thu, 09 Mar 2017) $
 *          $Revision: 11460 $
 *
 *  Copyright (c) 2013-2017 ARM Ltd., all rights reserved.
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

#include <string.h>
#include "wsf_types.h"
#include "bstream.h"
#include "wsf_trace.h"
#include "wsf_assert.h"
#include "wsf_efs.h"
#include "wsf_os.h"
#include "sec_api.h"
#include "svc_wdxs.h"
#include "wdxs_api.h"
#include "wdxs_main.h"
#include "dm_api.h"
#include "att_api.h"
#include "app_api.h"

#if WDXS_AU_ENABLED == TRUE

/* WDXS Authentication Control Block */
wdxsAuCb_t wdxsAuCb;

/*************************************************************************************************/
/*!
 *  \fn     wdxsAuSend
 *
 *  \brief  Transmit to authentication characteristic.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
void wdxsAuSend(dmConnId_t connId)
{
  APP_TRACE_INFO0("WDXS: AuSend");

  /* if notification enabled */
  if (AttsCccEnabled(connId, wdxsCb.auCccIdx))
  {
    /* send notification */
    AttsHandleValueNtf(connId, WDXS_AU_HDL, wdxsAuCb.auMsgLen, wdxsAuCb.auMsgBuf);
    wdxsCb.txReadyMask &= ~(WDXS_TX_MASK_AU_BIT | WDXS_TX_MASK_READY_BIT);

    wdxsAuCb.authState = WDXS_AU_STATE_WAIT_REPLY;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsAuSecComplete
 *
 *  \brief  Called by WDXS event handler when the WSF Sec operation is complete.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxsAuSecComplete(secAes_t *pAes)
{
  uint8_t *p = wdxsAuCb.auMsgBuf;

  /* Record the hash */
  memcpy(wdxsAuCb.auHash, pAes->pCiphertext, WDXS_AU_HASH_LEN);

  /* Build challenge message */
  UINT8_TO_BSTREAM(p, WDXS_AU_OP_CHALLENGE);
  memcpy(p, wdxsAuCb.auRand, WDXS_AU_RAND_LEN);
  wdxsAuCb.auMsgLen = WDXS_AU_RAND_LEN + WDXS_AU_HDR_LEN;

  /* Update State */
  wdxsAuCb.authState = WDXS_AU_STATE_WAIT_REPLY;

  /* Indicate TX Ready */
  wdxsCb.txReadyMask &= ~(WDXS_TX_MASK_AU_BIT | WDXS_TX_MASK_READY_BIT);
  WsfSetEvent(wdxsCb.handlerId, WDXS_EVT_TX_PATH);
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsAuProcStart
 *
 *  \brief  Process a request to start authentication.
 *
 *  \return None.
 */
/*************************************************************************************************/
static uint8_t wdxsAuProcStart(dmConnId_t connId, uint8_t len, uint8_t *pValue)
{
  /* Verify parameter length */
  if (len != WDXS_AU_PARAM_LEN_START)
  {
    return ATT_ERR_LENGTH;
  }

  /* Parse parameters */
  BSTREAM_TO_UINT8(wdxsAuCb.authLevel, pValue);
  BSTREAM_TO_UINT8(wdxsAuCb.authMode, pValue);

  /* Generate random number */
  SecRand(wdxsAuCb.auRand, WDXS_AU_RAND_LEN);

  /* Encrypt the random number to create the hash */
  SecAes(wdxsAuCb.sessionKey, wdxsAuCb.auRand, wdxsCb.handlerId, connId, WDXS_EVT_AU_SEC_COMPLETE);

  /* Update State */
  wdxsAuCb.authState = WDXS_AU_STATE_WAIT_SEC;

  return ATT_SUCCESS;
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsAuProcReply
 *
 *  \brief  Process a reply to the authentication challenge.
 *
 *  \return None.
 */
/*************************************************************************************************/
static uint8_t wdxsAuProcReply(uint8_t len, uint8_t *pValue)
{
  /* Verify parameter length */
  if (len != WDXS_AU_PARAM_LEN_REPLY)
  {
    return ATT_ERR_LENGTH;
  }

  if (wdxsAuCb.authState == WDXS_AU_STATE_WAIT_REPLY)
  {
    /* Verify [0-7] bytes of cipher text against what was sent by client */
    if (memcmp(wdxsAuCb.auHash, pValue, WDXS_AU_HASH_LEN) == 0)
    {
      /* Successful challenge */
      wdxsAuCb.authState = WDXS_AU_STATE_AUTHORIZED;
      return ATT_SUCCESS;
    }

    return WDXS_AU_ST_AUTH_FAILED;
  }

  return WDXS_AU_ST_INVALID_STATE;
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsAuWrite
 *
 *  \brief  Process a write to the authentication characteristic.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t wdxsAuWrite(dmConnId_t connId, uint16_t len, uint8_t *pValue)
{
  uint8_t op;
  uint8_t status;

  /* Sanity check message length */
  if (len < WDXS_AU_HDR_LEN)
  {
    return ATT_ERR_LENGTH;
  }

  /* Get Operation ID */
  BSTREAM_TO_UINT8(op, pValue);
  len -= WDXS_AU_HDR_LEN;

  APP_TRACE_INFO1("WDXS: AuWrite: op=%d", op);

  switch (op)
  {
    case WDXS_AU_OP_START:
      status = wdxsAuProcStart(connId, (uint8_t) len, pValue);
      break;

    case WDXS_AU_OP_REPLY:
      status = wdxsAuProcReply((uint8_t) len, pValue);
      break;

    default:
      status = ATT_ERR_RANGE;
      break;
  }

  return status;
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsAuthenticationCfg
 *
 *  \brief  Called at startup to configure WDXS authentication.
 *
 *  \param  reqLevel  Level of authentication that is required for a client to use WDXS
 *  \param  key       Authentication key (set to NULL if no authentication is required)
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxsAuthenticationCfg(bool_t reqLevel, uint8_t *pKey)
{
  wdxsAuCb.reqAuthLevel = reqLevel;

  if (pKey)
  {
    memcpy(wdxsAuCb.sessionKey, pKey, WDXS_AU_KEY_LEN);
  }
}

#endif /* WDXS_AU_ENABLED */
