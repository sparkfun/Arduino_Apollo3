/*************************************************************************************************/
/*!
 *  \file   sec_aes.c
 *        
 *  \brief  AES and random number security service implemented using HCI.
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

#include <string.h>
#include "wsf_types.h"
#include "wsf_queue.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "sec_api.h"
#include "sec_main.h"
#include "hci_api.h"
#include "calc128.h"

/**************************************************************************************************
  External Variables
**************************************************************************************************/

extern secCb_t secCb;

/*************************************************************************************************/
/*!
 *  \fn     getNextToken
 *        
 *  \brief  Returns the next token.
 *
 *  \return Token value.
 */
/*************************************************************************************************/
static uint8_t getNextToken()
{
  uint8_t token = secCb.token++;

  if (token == SEC_TOKEN_INVALID)
  {
    token = secCb.token++;
  }

  return token;
}

/*************************************************************************************************/
/*!
 *  \fn     SecAes
 *        
 *  \brief  Execute an AES calculation.  When the calculation completes, a WSF message will be
 *          sent to the specified handler.  This function returns a token value that
 *          the client can use to match calls to this function with messages.
 *
 *  \param  pKey        Pointer to 16 byte key.
 *  \param  pPlaintext  Pointer to 16 byte plaintext.
 *  \param  handlerId   WSF handler ID.
 *  \param  param       Client-defined parameter returned in message.
 *  \param  event       Event for client's WSF handler.
 *
 *  \return Token value.
 */
/*************************************************************************************************/
uint8_t SecAes(uint8_t *pKey, uint8_t *pPlaintext, wsfHandlerId_t handlerId,
               uint16_t param, uint8_t event)
{
  secQueueBuf_t  *pBuf;
  
  /* allocate a buffer */
  if ((pBuf = WsfMsgAlloc(sizeof(secQueueBuf_t))) != NULL)
  {
    pBuf->msg.hdr.status = getNextToken();
    pBuf->msg.hdr.param = param;
    pBuf->msg.hdr.event = event;
   
    pBuf->type = SEC_TYPE_AES;

    /* queue buffer */
    WsfMsgEnq(&secCb.queue, handlerId, pBuf);
    
    /* call HCI encrypt function */
    HciLeEncryptCmd(pKey, pPlaintext);

    return pBuf->msg.hdr.status;
  }
  
  return SEC_TOKEN_INVALID;
}

/*************************************************************************************************/
/*!
 *  \fn     SecAesHciCback
 *        
 *  \brief  Callback for HCI encryption for AES operations.
 *
 *  \param  pBuf        Pointer to sec queue element.
 *  \param  pEvent      Pointer to HCI event.
 *  \param  handlerId   WSF handler ID.
 *
 *  \return none.
 */
/*************************************************************************************************/
void SecAesHciCback(secQueueBuf_t *pBuf, hciEvt_t *pEvent, wsfHandlerId_t handlerId)
{
  secAes_t *pAes = (secAes_t *) &pBuf->msg;
  
  /* set encrypted data pointer and copy */
  pAes->pCiphertext = pBuf->ciphertext;
  Calc128Cpy(pAes->pCiphertext, pEvent->leEncryptCmdCmpl.data);

  /* send message */
  WsfMsgSend(handlerId, pAes);
}

/*************************************************************************************************/
/*!
 *  \fn     SecAesInit
 *        
 *  \brief  Called to initialize AES secuirity.
 *
 *  \param  none.
 *
 *  \return none.
 */
/*************************************************************************************************/
void SecAesInit()
{
  secCb.hciCbackTbl[SEC_TYPE_AES] = SecAesHciCback;
}
