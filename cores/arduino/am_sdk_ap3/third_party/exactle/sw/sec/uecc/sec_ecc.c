/*************************************************************************************************/
/*!
 *  \file   sec_ecc.c
 *        
 *  \brief  Security ECC implementation using uECC.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
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
#include "uECC.h"
#include "wstr.h"

#ifndef SEC_ECC_CFG
#define SEC_ECC_CFG SEC_ECC_CFG_UECC
#endif

#if SEC_ECC_CFG == SEC_ECC_CFG_UECC

/**************************************************************************************************
  External Variables
**************************************************************************************************/

extern secCb_t secCb;

/*************************************************************************************************/
/*!
 *  \fn     secEccRng
 *        
 *  \brief  Random number generator used by uECC.
 *
 *  \param  p_dest      Buffer to hold random number
 *  \param  p_size      Size of p_dest in bytes .
 *
 *  \return TRUE if successful.
 */
/*************************************************************************************************/
static int secEccRng(uint8_t *p_dest, unsigned p_size)
{
  SecRand(p_dest, p_size);
  return TRUE;
}

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
  /* TBD */
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
  secEccMsg_t *pMsg = WsfMsgAlloc(sizeof(secEccMsg_t));

  if (pMsg)
  {

    secEccKey_t temp_key;

    /* Generate the keys */ 
    uECC_make_key(temp_key.pubKey_x, temp_key.privKey, uECC_secp256r1());

    /* Reverse copy the public key (to big endian) */
    WStrReverseCpy(pMsg->data.key.pubKey_x, temp_key.pubKey_x, SEC_ECC_KEY_LEN);
    WStrReverseCpy(pMsg->data.key.pubKey_y, temp_key.pubKey_y, SEC_ECC_KEY_LEN);
    WStrReverseCpy(pMsg->data.key.privKey, temp_key.privKey, SEC_ECC_KEY_LEN);

    /* Send shared secret to handler */ 
    pMsg->hdr.event = event;
    pMsg->hdr.param = param;
    pMsg->hdr.status = HCI_SUCCESS;
    WsfMsgSend(handlerId, pMsg);

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
  secEccMsg_t *pMsg = WsfMsgAlloc(sizeof(secEccMsg_t));

  if (pMsg)
  {

    secEccKey_t           temp_ecc_key;
    uint8_t               temp_shared_key[SEC_ECC_KEY_LEN];

    /* Reverse keys (to little endian) */
    WStrReverseCpy(temp_ecc_key.pubKey_x, pKey->pubKey_x, SEC_ECC_KEY_LEN);
    WStrReverseCpy(temp_ecc_key.pubKey_y, pKey->pubKey_y, SEC_ECC_KEY_LEN);
    WStrReverseCpy(temp_ecc_key.privKey, pKey->privKey, SEC_ECC_KEY_LEN);

    if(uECC_valid_public_key(temp_ecc_key.pubKey_x, uECC_secp256r1()))
    {
      uECC_shared_secret(temp_ecc_key.pubKey_x, temp_ecc_key.privKey, temp_shared_key, uECC_secp256r1());

      /* Reverse copy the DH key (to big endian) */
      WStrReverseCpy(pMsg->data.sharedSecret.secret, temp_shared_key, SEC_ECC_KEY_LEN);
     
    }
    else
    {
      memset(pMsg->data.sharedSecret.secret, 0xFF, SEC_ECC_KEY_LEN);
    }

    /* Send shared secret to handler */  
    pMsg->hdr.event = event;
    pMsg->hdr.param = param;
    pMsg->hdr.status = HCI_SUCCESS;
    WsfMsgSend(handlerId, pMsg);

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
  uECC_set_rng(secEccRng);
}

#endif /* SEC_ECC_CFG */
