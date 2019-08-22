/*************************************************************************************************/
/*!
 *  \file   hci_main.c
 *
 *  \brief  HCI main module.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
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

#include "wsf_types.h"
#include "wsf_msg.h"
#include "hci_api.h"
#include "hci_main.h"

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/* Control block */
hciCb_t hciCb;

/*************************************************************************************************/
/*!
 *  \fn     HciEvtRegister
 *        
 *  \brief  Register a callback for HCI events.
 *
 *  \param  evtCback  Callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciEvtRegister(hciEvtCback_t evtCback)
{
  hciCb.evtCback = evtCback;
}

/*************************************************************************************************/
/*!
 *  \fn     HciSecRegister
 *        
 *  \brief  Register a callback for certain HCI security events.
 *
 *  \param  secCback  Callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciSecRegister(hciSecCback_t secCback)
{
  hciCb.secCback = secCback;
}

/*************************************************************************************************/
/*!
 *  \fn     HciAclRegister
 *        
 *  \brief  Register callbacks for the HCI data path.
 *
 *  \param  aclCback  ACL data callback function.
 *  \param  flowCback Flow control callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciAclRegister(hciAclCback_t aclCback, hciFlowCback_t flowCback)
{
  hciCb.aclCback = aclCback;
  hciCb.flowCback = flowCback;
}


/*************************************************************************************************/
/*!
 *  \fn     HciHandlerInit
 *        
 *  \brief  HCI handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID for HCI.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciHandlerInit(wsfHandlerId_t handlerId)
{
  /* store handler ID */
  hciCb.handlerId = handlerId;

  /* init rx queue */
  WSF_QUEUE_INIT(&hciCb.rxQueue);
    
  /* perform other hci initialization */
  HciCoreInit();
}

/*************************************************************************************************/
/*!
 *  \fn     HciHandler
 *        
 *  \brief  WSF event handler for HCI.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
  HciCoreHandler(event, pMsg);
}
