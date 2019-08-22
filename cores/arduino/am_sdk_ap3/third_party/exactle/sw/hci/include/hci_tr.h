/*************************************************************************************************/
/*!
 *  \file   hci_tr.h
 *
 *  \brief  HCI transport interface.
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
#ifndef HCI_TR_H
#define HCI_TR_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void hciTrSendAclData(void *pContext, uint8_t *pAclData);
void hciTrSendCmd(uint8_t *pCmdData);
bool_t hciTrInit(uint8_t port, uint32_t baudRate, bool_t flowControl);
void hciTrShutdown(void);

#ifdef __cplusplus
};
#endif

#endif /* HCI_TR_H */
