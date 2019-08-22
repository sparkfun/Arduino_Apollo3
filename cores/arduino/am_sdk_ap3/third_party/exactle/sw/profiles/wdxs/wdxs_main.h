/*************************************************************************************************/
/*!
 *  \file   wdxs_main.h
 *
 *  \brief  Wireless Data Exchange profile implementation.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
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

#ifndef WDXS_MAIN_H
#define WDXS_MAIN_H

/**************************************************************************************************
  Macros
**************************************************************************************************/

/* Default Feature Set */
#ifndef WDXS_DC_ENABLED
#define WDXS_DC_ENABLED             TRUE
#endif

#ifndef WDXS_AU_ENABLED
#define WDXS_AU_ENABLED             TRUE
#endif

#ifndef WDXS_OTA_ENABLED
#define WDXS_OTA_ENABLED            TRUE
#endif

/* Maximum file transfer data payload */
#define WDXS_FTD_PAYLOAD_MAX        (ATT_DEFAULT_PAYLOAD_LEN - 1)

/* Special length for streaming file */
#define WDXS_STREAM_FILE_LEN        0xFFFFFFFF

/*! WSF event types for application event handler */
#define WDXS_EVT_TX_PATH            0x01      /*! Trigger tx data path */
#define WDXS_EVT_AU_SEC_COMPLETE    0x02      /*! AU encryption of challenge ready */

/* TX Ready Mask Bits */
#define WDXS_TX_MASK_READY_BIT    (1<<0)
#define WDXS_TX_MASK_DC_BIT       (1<<1)
#define WDXS_TX_MASK_FTC_BIT      (1<<2)
#define WDXS_TX_MASK_FTD_BIT      (1<<3)
#define WDXS_TX_MASK_AU_BIT       (1<<4)

/*! Authentication states */
#define WDXS_AU_STATE_UNAUTHORIZED      0x00        /*! Authentication has not started */
#define WDXS_AU_STATE_HASHING           0x01        /*! Authentication hash is being calculated */
#define WDXS_AU_STATE_WAIT_SEC          0x02        /*! Authentication challenge sent */
#define WDXS_AU_STATE_WAIT_REPLY        0x03        /*! Authentication waiting for challenge reply */
#define WDXS_AU_STATE_AUTHORIZED        0x04        /*! Authentication completed successfully */

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! WDXS profile control block */
typedef struct
{
  wsfHandlerId_t    handlerId;        /* WSF handler ID */
  uint8_t           txReadyMask;      /* Bits indicate DC, FTC, FTD, and/or AU wish to transmit */

  /* connection parameters */
  uint16_t          connInterval;     /* connection interval */
  uint16_t          connLatency;      /* connection latency */
  uint16_t          supTimeout;       /* supervision timeout */

  /* for file transfer */
  uint32_t          ftOffset;         /* file data offset */
  uint32_t          ftLen;            /* remaining data length for current operation */
  uint32_t          ftTotalLen;       /* file total length */
  uint16_t          ftHandle;         /* file handle */
  uint16_t          ftcMsgLen;        /* message length */
  uint8_t           ftcMsgBuf[ATT_DEFAULT_PAYLOAD_LEN]; /* message buffer */
  uint8_t           ftInProgress;     /* operation in progress */
  uint8_t           ftPrefXferType;   /* Preferred transport type */

  /* ccc index */
  uint8_t          dcCccIdx;          /* device configuration ccc index */
  uint8_t          auCccIdx;          /* authentication ccc index */
  uint8_t          ftcCccIdx;         /* file transfer control ccc index */
  uint8_t          ftdCccIdx;         /* file transfer data ccc index */
} wdxsCb_t;

/* WDXS Device Configuration Control Block */
typedef struct
{
  uint16_t          dcMsgLen;                           /* message length */
  uint8_t           dcMsgBuf[ATT_DEFAULT_PAYLOAD_LEN];  /* message buffer */
  bool_t            doReset;                            /* Reset device after disconnect */
} wdxsDcCb_t;

/* WDXS Authentication Control Block */
typedef struct
{
  uint8_t           auMsgBuf[ATT_DEFAULT_PAYLOAD_LEN];  /* message buffer */
  uint8_t           auRand[WDXS_AU_RAND_LEN];           /* random challenge */
  uint8_t           sessionKey[WDXS_AU_KEY_LEN];        /* session key */
  uint8_t           auHash[WDXS_AU_HASH_LEN];              /* session key */
  uint16_t          auMsgLen;                           /* message length */
  uint8_t           authLevel;                          /* current authentication level */
  uint8_t           authMode;                           /* current authentication mode */
  uint8_t           reqAuthLevel;                       /* requested authentication level */
  uint8_t           authState;                          /* authentication protocol state */
} wdxsAuCb_t;

typedef union
{
  wsfMsgHdr_t       hdr;
  dmEvt_t           dm;
  attsCccEvt_t      ccc;
} wdxsMsg_t;

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/* WDXS Control Block External Declaration */
extern wdxsCb_t wdxsCb;
extern wdxsAuCb_t wdxsAuCb;
extern wdxsDcCb_t wdxsDcCb;

/**************************************************************************************************
  Global Function Prototypes
**************************************************************************************************/
void wdxsDcSend(dmConnId_t connId);
void wdxsFtcSend(dmConnId_t connId);
void wdxsFtdSend(dmConnId_t connId);
void wdxsAuSend(dmConnId_t connId);

uint8_t wdxsDcWrite(dmConnId_t connId, uint16_t len, uint8_t *pValue);
uint8_t wdxsFtcWrite(dmConnId_t connId, uint16_t len, uint8_t *pValue);
uint8_t wdxsFtdWrite(dmConnId_t connId, uint16_t len, uint8_t *pValue);
uint8_t wdxsAuWrite(dmConnId_t connId, uint16_t len, uint8_t *pValue);

uint8_t wdxsDcUpdateConnParam(dmConnId_t connId, uint8_t status);

void WdxsUpdateListing(void);

#endif /* WDXS_MAIN_H */
