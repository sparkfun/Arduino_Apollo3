/*************************************************************************************************/
/*!
 *  \file   sec_main.h
 *
 *  \brief  Internal security service structures.
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
#ifndef SEC_MAIN_H
#define SEC_MAIN_H

#include "hci_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/


/*! AES, CMAC and HCI algorithm block length */
#define SEC_BLOCK_LEN             16

/* CMAC constant Rb */
#define SEC_CMAC_RB               0x87

/*! Multiple of HCI_RAND_LEN to keep in the secCb_t rand data buffer */
#define SEC_HCI_RAND_MULT         (32 / HCI_RAND_LEN)

/*! Compile time ECC configuration */
#define SEC_ECC_CFG_DEBUG         0
#define SEC_ECC_CFG_UECC          1
#define SEC_ECC_CFG_HCI           2

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! Enumeration of security operation types */
enum
{
  SEC_TYPE_AES,
  SEC_TYPE_CMAC,
  SEC_TYPE_DH,
  SEC_NUM_TYPES
};

/*! Security queue element for CMAC operations */
typedef struct
{
  uint8_t        *pPlainText;
  uint8_t        key[SEC_CMAC_KEY_LEN];
  uint8_t        subkey[SEC_CMAC_KEY_LEN];
  uint16_t       position;
  uint16_t       len;
  wsfHandlerId_t handlerId;
  uint8_t        state;
} secCmacSecCb_t;

/*! Security queue element */
typedef struct
{
  secMsg_t       msg;
  uint8_t        ciphertext[SEC_BLOCK_LEN];
  uint8_t        reserved[SEC_BLOCK_LEN];
  void           *pCb;
  uint8_t        type;
} secQueueBuf_t;

typedef void secHciCback_t(secQueueBuf_t *pBuf, hciEvt_t *pEvent, wsfHandlerId_t handlerId);
typedef secHciCback_t *pSecHciCback_t;

/* Control block */
typedef struct
{
  uint8_t        rand[HCI_RAND_LEN * SEC_HCI_RAND_MULT]; /* Random data buffer */
  wsfQueue_t     queue;                                  /* Queue for AES requests */
  uint8_t        token;                                  /* Token value */
  pSecHciCback_t hciCbackTbl[SEC_NUM_TYPES];
} secCb_t;

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

#ifdef __cplusplus
};
#endif

#endif /* SEC_MAIN_H */
