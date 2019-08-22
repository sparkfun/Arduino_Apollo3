/*************************************************************************************************/
/*!
 *  \file   sec_main.c
 *        
 *  \brief  Security service implemented using HCI.
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
#include "wsf_assert.h"
#include "sec_api.h"
#include "sec_main.h"
#include "hci_api.h"
#include "calc128.h"

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/* Security Control block */
secCb_t secCb;

/*************************************************************************************************/
/*!
 *  \fn     secHciCback
 *        
 *  \brief  Callback for HCI encryption and random number events.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void secHciCback(hciEvt_t *pEvent)
{
  secQueueBuf_t  *pBuf  = NULL;
  wsfHandlerId_t    handlerId = 0;
  
  /* handle random number event */
  if (pEvent->hdr.event == HCI_LE_RAND_CMD_CMPL_CBACK_EVT)
  {
    /* move up data by eight bytes */    
    memmove(&secCb.rand[HCI_RAND_LEN], secCb.rand, HCI_RAND_LEN * (SEC_HCI_RAND_MULT-1));
    
    /* copy new data to random data buffer */
    memcpy(secCb.rand, pEvent->leRandCmdCmpl.randNum, HCI_RAND_LEN);
  }
  /* handle encryption event */
  else if (pEvent->hdr.event == HCI_LE_ENCRYPT_CMD_CMPL_CBACK_EVT ||
           pEvent->hdr.event == HCI_LE_READ_LOCAL_P256_PUB_KEY_CMPL_CBACK_EVT ||
           pEvent->hdr.event == HCI_LE_GENERATE_DHKEY_CMPL_CBACK_EVT)
  {
    if ((pBuf = WsfMsgDeq(&secCb.queue, &handlerId)) != NULL)
    {
      /* Process HCI callback based on type of encryption */ 
      if (secCb.hciCbackTbl[pBuf->type])
      {
        secCb.hciCbackTbl[pBuf->type](pBuf, pEvent, handlerId);
      }
      else
      {
        WSF_TRACE_WARN0("WSF sec not registered");
      }
    }
    else
    {
      WSF_TRACE_WARN0("WSF sec queue empty!");
    }
  }
  else if (pEvent->hdr.event == HCI_HW_ERROR_CBACK_EVT)
  {
    while((pBuf = WsfMsgDeq(&secCb.queue, &handlerId)) != NULL)
    {
      WsfMsgFree(pBuf);
    }  
    
    secCb.token = 0;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     SecInit
 *        
 *  \brief  Initialize the security service.  This function should only be called once
 *          upon system initialization.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SecInit(void)
{
  WSF_QUEUE_INIT(&secCb.queue);
  secCb.token = 0;
  
  /* Register callback with HCI */
  HciSecRegister(secHciCback);
}

/*************************************************************************************************/
/*!
 *  \fn     SecRandInit
 *        
 *  \brief  Initialize the random number service.  This function should only be called once
 *          upon system initialization.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SecRandInit(void)
{
  int8_t i;

  /* get new random numbers */
  for (i=0; i<SEC_HCI_RAND_MULT; i++)
  {
    HciLeRandCmd();
  }
}

/*************************************************************************************************/
/*!
 *  \fn     SecRand
 *        
 *  \brief  This function returns up to 16 bytes of random data to a buffer provided by the
 *          client.
 *
 *  \param  pRand       Pointer to returned random data.
 *  \param  randLen     Length of random data.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SecRand(uint8_t *pRand, uint8_t randLen)
{
  int8_t count = (randLen + HCI_RAND_LEN - 1) / HCI_RAND_LEN;

  WSF_ASSERT(randLen <= HCI_RAND_LEN * SEC_HCI_RAND_MULT);

  /* copy data */
  memcpy(pRand, secCb.rand, randLen);
  
  /* get new random numbers */
  while (count--)
  {
    HciLeRandCmd();
  }
}
