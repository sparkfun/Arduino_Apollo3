/*************************************************************************************************/
/*!
 *  \file   sec_ecc_hci.c
 *
 *  \brief  WSF Security ECC implementation using HCI.
 *
 *          $Date: 2015-10-15 14:57:57 -0400 (Thu, 15 Oct 2015) $
 *          $Revision: 4218 $
 *
 *  Copyright (c) 2010-2017 ARM Ltd., all rights reserved.
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

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "wsf_types.h"
#include "wsf_queue.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "sec_api.h"
#include "sec_main.h"
#include "wsf_buf.h"
#include "hci_api.h"
#include "calc128.h"
#include "wstr.h"
#include "uECC.h"
#include "am_mcu_apollo.h"

#ifndef SEC_ECC_CFG
#define SEC_ECC_CFG SEC_ECC_CFG_HCI
#endif

#if SEC_ECC_CFG == SEC_ECC_CFG_HCI

/**************************************************************************************************
External Variables
**************************************************************************************************/

extern secCb_t secCb;

/**************************************************************************************************
Local Variables
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     SecEccHciCback
 *
 *  \brief  Callback for HCI encryption for ECC operations.
 *
 *  \param  pBuf        Pointer to sec queue element.
 *  \param  pEvent      Pointer to HCI event.
 *  \param  handlerId   WSF handler ID.
 *
 *  \return none.
 */
/*************************************************************************************************/
void SecEccHciCback(secQueueBuf_t *pBuf, hciEvt_t *pEvent, wsfHandlerId_t handlerId)
{
  secEccMsg_t *pMsg = (secEccMsg_t *) &pBuf->msg;

  if (pEvent->hdr.event == HCI_LE_READ_LOCAL_P256_PUB_KEY_CMPL_CBACK_EVT)
  {
    /* Reverse copy the public key (to big endian) */
    WStrReverseCpy(pMsg->data.key.pubKey_x, pEvent->leP256.key, SEC_ECC_KEY_LEN);
    WStrReverseCpy(pMsg->data.key.pubKey_y, pEvent->leP256.key + SEC_ECC_KEY_LEN, SEC_ECC_KEY_LEN);

    /* Send shared secret to handler */
    pMsg->hdr.status = pEvent->leP256.status;
    WsfMsgSend(handlerId, pMsg);
  }
  else if (pEvent->hdr.event == HCI_LE_GENERATE_DHKEY_CMPL_CBACK_EVT)
  {
    /* Reverse copy the DH key (to big endian) */
    WStrReverseCpy(pMsg->data.sharedSecret.secret, pEvent->leGenDHKey.key, SEC_ECC_KEY_LEN);

    // Invalid point from remote will return HCI_ERR_INVALID_PARAM as per Core Spec Errata (10734)
    if(pEvent->leGenDHKey.status != HCI_SUCCESS) {
      // all octets of the DHKey event parameter should be set to 0xFF
      memset(pMsg->data.sharedSecret.secret, 0xFF, SEC_ECC_KEY_LEN);
    }
    /* Send shared secret to handler */
    pMsg->hdr.status = pEvent->leGenDHKey.status;
    WsfMsgSend(handlerId, pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     SecEccGenKey
 *
 *  \brief  Generate an ECC key.
 *
 *  \param  handlerId   WSF handler ID for client.
 *  \param  param       Optional parameter sent to client's WSF handler.
 *  \param  event       Event for client's WSF handler.
 *
 *  \return TRUE if successful, else FALSE.
 */
/*************************************************************************************************/
bool_t SecEccGenKey(wsfHandlerId_t handlerId, uint16_t param, uint8_t event)
{
  secQueueBuf_t *pBuf;
  uint16_t bufSize = sizeof(secQueueBuf_t) + sizeof(secEccMsg_t);

  if ((pBuf = WsfMsgAlloc(bufSize)) != NULL)
  {
    /* Record the event and parameter for use in the HCI response */
    pBuf->msg.hdr.param = param;
    pBuf->msg.hdr.event = event;
    pBuf->type = SEC_TYPE_DH;

    /* queue buffer */
    WsfMsgEnq(&secCb.queue, handlerId, pBuf);

    /* Request the local public key via HCI */
    HciLeReadLocalP256PubKey();

    return TRUE;
  }

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     SecEccGenSharedSecret
 *
 *  \brief  Generate an ECC key.
 *
 *  \param  pKey        ECC Key structure.
 *  \param  handlerId   WSF handler ID for client.
 *  \param  param       Optional parameter sent to client's WSF handler.
 *  \param  event       Event for client's WSF handler.
 *
 *  \return TRUE if successful, else FALSE.
 */
/*************************************************************************************************/
bool_t SecEccGenSharedSecret(secEccKey_t *pKey, wsfHandlerId_t handlerId, uint16_t param, uint8_t event)
{
  secQueueBuf_t *pBuf;
  uint16_t bufSize = sizeof(secQueueBuf_t) + sizeof(secEccMsg_t);

  if ((pBuf = WsfMsgAlloc(bufSize)) != NULL)
  {
	  secEccKey_t           temp_ecc_key;

    /* Reverse keys (to little endian) */
    WStrReverseCpy(temp_ecc_key.pubKey_x, pKey->pubKey_x, SEC_ECC_KEY_LEN);
    WStrReverseCpy(temp_ecc_key.pubKey_y, pKey->pubKey_y, SEC_ECC_KEY_LEN);
	
    #if defined(AM_PART_APOLLO) || defined(AM_PART_APOLLO2)
    {
    #else
    if(!APOLLO3_B0) 
    {
    #endif
      // at this time only B0 has fix for the security vulnerability.
      if(!uECC_valid_public_key(temp_ecc_key.pubKey_x, uECC_secp256r1()))
      {

        WsfMsgFree(pBuf);

        secEccMsg_t *pMsg = WsfMsgAlloc(sizeof(secEccMsg_t));

        if (pMsg)
        {
          // set invalid shared key as suggested by the security errata
          memset(pMsg->data.sharedSecret.secret, 0xFF, SEC_ECC_KEY_LEN);
          /* Send shared secret to handler */  
          pMsg->hdr.event = event;
          pMsg->hdr.param = param;
          pMsg->hdr.status = HCI_SUCCESS;
          WsfMsgSend(handlerId, pMsg);
          return TRUE;
        }
        else {
          return FALSE;
        }
      }
    }

    // 
    // At this point either remote public key is valid or controller has
    // the fix for the security vulnerability.
    // 

    /* Record the event and parameter for use in the HCI response */
    pBuf->msg.hdr.param = param;
    pBuf->msg.hdr.event = event;
    pBuf->type = SEC_TYPE_DH;

    /* queue buffer */
    WsfMsgEnq(&secCb.queue, handlerId, pBuf);

    /* Request the DH Key via HCI */
    HciLeGenerateDHKey(temp_ecc_key.pubKey_x, temp_ecc_key.pubKey_y);

    return TRUE;
  }

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     SecEccInit
 *
 *  \brief  Called to initialize ECC security.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SecEccInit()
{
  secCb.hciCbackTbl[SEC_TYPE_DH] = SecEccHciCback;
}

#endif /* SEC_ECC_CFG */
