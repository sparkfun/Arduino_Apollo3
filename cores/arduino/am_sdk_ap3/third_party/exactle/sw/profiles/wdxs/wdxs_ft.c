/*************************************************************************************************/
/*!
 *  \file   wdxs_ft.c
 *
 *  \brief  Wireless Data Exchange profile implementation - File Transfer.
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
#include <stddef.h>
#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_assert.h"
#include "wsf_efs.h"
#include "bstream.h"
#include "svc_wdxs.h"
#include "wdxs_api.h"
#include "wdxs_main.h"
#include "dm_api.h"
#include "app_api.h"

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     wdxsFileRead
 *
 *  \brief  Read data from file.
 *
 *  \return TRUE if EOF.
 */
/*************************************************************************************************/
static bool_t wdxsFileRead(uint16_t handle, uint32_t offset, uint32_t *pReadLen, uint8_t *pData)
{
  bool_t eof = FALSE;
  uint32_t fileSize = WsfEfsGetFileSize(handle);

  if (fileSize && (offset + *pReadLen > fileSize))
  {
    *pReadLen = fileSize - offset;
    eof = TRUE;
  }

  WsfEfsGet(handle, offset, pData, (uint16_t) *pReadLen);

  return eof;
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsInitializeForPut
 *
 *  \brief  Prepare for FTD data.
 *
 *  \return None.
 */
/*************************************************************************************************/
static uint8_t wdxsInitializeForPut(dmConnId_t connId, uint16_t handle)
{
  uint32_t availableSize = WsfEfsGetFileMaxSize(handle);

  /* verify file total length
   * verify offset+length is not more than total length
   */
  if ((wdxsCb.ftTotalLen > availableSize) ||
      ((wdxsCb.ftOffset + wdxsCb.ftLen) > wdxsCb.ftTotalLen))
  {
    return WDXS_FTC_ST_INVALID_OP_DATA;
  }

  /* Erase on offset of zero */
  if (wdxsCb.ftOffset == 0)
  {
    WsfEfsErase(handle);
  }

  /* set up file put operation */
  wdxsCb.ftHandle = handle;
  wdxsCb.ftInProgress = WDXS_FTC_OP_PUT_REQ;

  return WDXS_FTC_ST_SUCCESS;
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtcSend
 *
 *  \brief  Send a file transfer control characteristic notification.
 *
 *  \return None.
 */
/*************************************************************************************************/
void wdxsFtcSend(dmConnId_t connId)
{
  APP_TRACE_INFO0("WDXS: FTC Send");

  /* if notification enabled */
  if (AttsCccEnabled(connId, wdxsCb.ftcCccIdx))
  {
    /* send notification */
    AttsHandleValueNtf(connId, WDXS_FTC_HDL, wdxsCb.ftcMsgLen, wdxsCb.ftcMsgBuf);
    wdxsCb.txReadyMask &= ~(WDXS_TX_MASK_FTC_BIT | WDXS_TX_MASK_READY_BIT);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtcSendRsp
 *
 *  \brief  Send a file transfer response message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void wdxsFtcSendRsp(dmConnId_t connId, uint8_t op, uint16_t handle, uint8_t status)
{
  uint8_t *p;

  /* there should not be another response message set up */
  if (wdxsCb.txReadyMask & WDXS_TX_MASK_FTC_BIT)
  {
    APP_TRACE_WARN0("WDXS: FTC message overflow");
    return;
  }

  APP_TRACE_INFO3("WDXS: FTC SendRsp op=%d handle=%d status=%d", op, handle, status);

  /* build message */
  p = wdxsCb.ftcMsgBuf;
  UINT8_TO_BSTREAM(p, op);
  UINT16_TO_BSTREAM(p, handle);

  if (op != WDXS_FTC_OP_ABORT && op != WDXS_FTC_OP_EOF)
  {
    UINT8_TO_BSTREAM(p, status);
  }

  if (op == WDXS_FTC_OP_GET_RSP || op == WDXS_FTC_OP_PUT_RSP)
  {
    UINT8_TO_BSTREAM(p, WDXS_FTC_TRANSPORT_TYPE);
    UINT16_TO_BSTREAM(p, WDXS_FTC_TRANSPORT_ID);
  }

  wdxsCb.ftcMsgLen = (uint16_t) (p - wdxsCb.ftcMsgBuf);

  /* Indicate TX Ready */
  wdxsCb.txReadyMask |= WDXS_TX_MASK_FTC_BIT;
  WsfSetEvent(wdxsCb.handlerId, WDXS_EVT_TX_PATH);
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtcProcGetReq
 *
 *  \brief  Process a file get request.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void wdxsFtcProcGetReq(dmConnId_t connId, uint16_t handle, uint16_t len, uint8_t *pValue)
{
  uint8_t status;

  APP_TRACE_INFO2("WDXS: FTC GetReq handle=%d len=%d", handle, len);

  /* verify operation not already in progress */
  if (wdxsCb.ftInProgress != WDXS_FTC_OP_NONE)
  {
    status = WDXS_FTC_ST_IN_PROGRESS;
  }
  else if ((WsfEfsGetFilePermissions(handle) & WSF_EFS_REMOTE_GET_PERMITTED) == 0)
  {
    status = WDXS_FTC_ST_INVALID_OP_FILE;
  }
  else
  {
    if (handle == WDXS_FLIST_HANDLE)
    {
      WdxsUpdateListing();
    }

    /* parse operation data */
    BSTREAM_TO_UINT32(wdxsCb.ftOffset, pValue);
    BSTREAM_TO_UINT32(wdxsCb.ftLen, pValue);
    BSTREAM_TO_UINT8(wdxsCb.ftPrefXferType, pValue);

    /* set up file get operation */
    wdxsCb.ftHandle = handle;
    wdxsCb.ftInProgress = WDXS_FTC_OP_GET_REQ;

    wdxsCb.txReadyMask |= WDXS_TX_MASK_FTD_BIT;
    WsfSetEvent(wdxsCb.handlerId, WDXS_EVT_TX_PATH);

    status = WDXS_FTC_ST_SUCCESS;
  }

  /* send response */
  wdxsFtcSendRsp(connId, WDXS_FTC_OP_GET_RSP, handle, status);
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtcProcPutReq
 *
 *  \brief  Process a file put request.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void wdxsFtcProcPutReq(dmConnId_t connId, uint16_t handle, uint16_t len, uint8_t *pValue)
{
  uint8_t status;

  /* verify operation not already in progress */
  if (wdxsCb.ftInProgress != WDXS_FTC_OP_NONE)
  {
    status = WDXS_FTC_ST_IN_PROGRESS;
  }
  /* verify permissions */
  else if ((WsfEfsGetFilePermissions(handle) & WSF_EFS_REMOTE_PUT_PERMITTED) == 0)
  {
    status = WDXS_FTC_ST_INVALID_HANDLE;
  }
  else
  {
    /* parse operation data */
    wdxsCb.ftHandle = handle;
    BSTREAM_TO_UINT32(wdxsCb.ftOffset, pValue);
    BSTREAM_TO_UINT32(wdxsCb.ftLen, pValue);
    BSTREAM_TO_UINT32(wdxsCb.ftTotalLen, pValue);
    BSTREAM_TO_UINT8(wdxsCb.ftPrefXferType, pValue);

    APP_TRACE_INFO3("WDXS: FTC PutReq handle=%d offset=%d, len=%d", handle,
                     wdxsCb.ftOffset, wdxsCb.ftLen);

    /* Initialize transfer*/
    status = wdxsInitializeForPut(connId, handle);
  }

  APP_TRACE_INFO2("WDXS: FTC PutReq handle=%d status=%d", handle, status);

  /* send response */
  wdxsFtcSendRsp(connId, WDXS_FTC_OP_PUT_RSP, handle, status);
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtcProcVerifyReq
 *
 *  \brief  Process a file verify request.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void wdxsFtcProcVerifyReq(dmConnId_t connId, uint16_t handle)
{
  uint8_t status;

  APP_TRACE_INFO1("WDXS: FTC VerifyReq: handle=%d", handle);

  /* verify operation not already in progress */
  if (wdxsCb.ftInProgress != WDXS_FTC_OP_NONE)
  {
    status = WDXS_FTC_ST_IN_PROGRESS;
  }
  else if ((WsfEfsGetFilePermissions(handle) & WSF_EFS_REMOTE_VERIFY_PERMITTED) == 0)
  {
    status = WDXS_FTC_ST_INVALID_HANDLE;
  }
  else
  {
    /* Call the media specific validate command */
    status = WsfEfsMediaSpecificCommand(handle, WSF_EFS_VALIDATE_CMD, 0);
  }

  /* send response */
  wdxsFtcSendRsp(connId, WDXS_FTC_OP_VERIFY_RSP, handle, status);
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtcProcEraseReq
 *
 *  \brief  Process a file erase request.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void wdxsFtcProcEraseReq(dmConnId_t connId, uint16_t handle)
{
  uint8_t status = WDXS_FTC_ST_SUCCESS;

  APP_TRACE_INFO1("WDXS: FTC EraseReq: handle=%d", handle);

  /* verify operation not already in progress */
  if (wdxsCb.ftInProgress != WDXS_FTC_OP_NONE)
  {
    status = WDXS_FTC_ST_IN_PROGRESS;
  }
  /* verify file handle */
  else if ((WsfEfsGetFilePermissions(handle) & WSF_EFS_REMOTE_ERASE_PERMITTED) == 0)
  {
    status = WDXS_FTC_ST_INVALID_OP_FILE;
  }
  else
  {
    /* do file erase */
    WsfEfsErase(handle);
  }

  /* send response */
  wdxsFtcSendRsp(connId, WDXS_FTC_OP_ERASE_RSP, handle, status);
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtcProcAbort
 *
 *  \brief  Process a file abort.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void wdxsFtcProcAbort(dmConnId_t connId, uint16_t handle)
{
  APP_TRACE_INFO1("WDXS: FTC AbortReq: handle=%d", handle);

  if (wdxsCb.ftInProgress != WDXS_FTC_OP_NONE)
  {
    /* abort operation */
    if (WsfEfsGetFileType(handle) == WSF_EFS_FILE_TYPE_STREAM)
    {
      wdxsCb.ftInProgress = WDXS_FTC_OP_ABORT;
    }
    else
    {
      wdxsCb.ftInProgress = WDXS_FTC_OP_NONE;
    }

    wdxsCb.ftLen = 0;
    wdxsCb.ftOffset = 0;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtdWrite
 *
 *  \brief  Process a write to the file transfer data characteristic.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t wdxsFtdWrite(dmConnId_t connId, uint16_t len, uint8_t *pValue)
{
  /* verify put operation in progress */
  if (wdxsCb.ftInProgress != WDXS_FTC_OP_PUT_REQ)
  {
    return ATT_ERR_UNLIKELY;
  }

  /* verify data length */
  if (len <= WDXS_FTD_HDR_LEN)
  {
    return ATT_ERR_LENGTH;
  }

  /* verify more data is expected */
  if (wdxsCb.ftLen >= len)
  {
    WsfEfsPut(wdxsCb.ftHandle, wdxsCb.ftOffset, pValue, len);

    /* update remaining length of put request */
    wdxsCb.ftOffset += len;
    wdxsCb.ftLen -= len;

    /* if end of put req reached */
    if (wdxsCb.ftLen == 0)
    {
      if (wdxsCb.ftOffset == wdxsCb.ftTotalLen)
      {
        /* Call the media specific WDXS Put Complete command */
        WsfEfsMediaSpecificCommand(wdxsCb.ftHandle, WSF_EFS_WDXS_PUT_COMPLETE_CMD, wdxsCb.ftTotalLen);
      }

      /* put req done */
      wdxsCb.ftInProgress = WDXS_FTC_OP_NONE;

      /* send eof */
      wdxsFtcSendRsp(connId, WDXS_FTC_OP_EOF, wdxsCb.ftHandle, 0);
    }
  }

  return ATT_SUCCESS;
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtcWrite
 *
 *  \brief  Process a write to the file transfer control characteristic.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t wdxsFtcWrite(dmConnId_t connId, uint16_t len, uint8_t *pValue)
{
  uint8_t   op;
  uint16_t  handle;

  APP_TRACE_INFO1("WDXS: FTC Write: len=%d", len);

  /* sanity check on message length */
  if (len < WDXS_FTC_HDR_LEN + WDXS_FTC_HANDLE_LEN)
  {
    return ATT_ERR_LENGTH;
  }

  /* get operation and file handle */
  BSTREAM_TO_UINT8(op, pValue);
  BSTREAM_TO_UINT16(handle, pValue);

  APP_TRACE_INFO2("WDXS: FTC Write: op=%d handle=%d", op, handle);

  len -= WDXS_FTC_HANDLE_LEN + WDXS_FTC_HDR_LEN;

  switch (op)
  {
    case WDXS_FTC_OP_GET_REQ:
      wdxsFtcProcGetReq(connId, handle, len, pValue);
      break;

    case WDXS_FTC_OP_PUT_REQ:
      wdxsFtcProcPutReq(connId, handle, len, pValue);
      break;

    case WDXS_FTC_OP_VERIFY_REQ:
      wdxsFtcProcVerifyReq(connId, handle);
      break;

    case WDXS_FTC_OP_ERASE_REQ:
      wdxsFtcProcEraseReq(connId, handle);
      break;

    case WDXS_FTC_OP_ABORT:
      wdxsFtcProcAbort(connId, handle);
      break;

    default:
      break;
  }

  return ATT_SUCCESS;
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsFtdSend
 *
 *  \brief  Send a file transfer data characteristic notification.
 *
 *  \return None.
 */
/*************************************************************************************************/
void wdxsFtdSend(dmConnId_t connId)
{
  /* if notification enabled */
  if (AttsCccEnabled(connId, wdxsCb.ftdCccIdx))
  {
    uint8_t   buf[ATT_DEFAULT_PAYLOAD_LEN];
    uint8_t   *pBuf = buf;
    uint32_t  readLen = WDXS_FTD_PAYLOAD_MAX;
    bool_t    eof;
    uint8_t   fileType = WsfEfsGetFileType(wdxsCb.ftHandle);

    /* Check for abort when Streaming */
    if ((fileType == WSF_EFS_FILE_TYPE_STREAM) && (wdxsCb.ftInProgress == WDXS_FTC_OP_ABORT))
    {
      eof = TRUE;
      readLen = 0;
    }

    /* read data from file */
    readLen = (readLen < wdxsCb.ftLen) ? readLen : wdxsCb.ftLen;

    if (readLen)
    {
      eof = wdxsFileRead(wdxsCb.ftHandle, wdxsCb.ftOffset, &readLen, pBuf);
    }

    /* if read data length not zero */
    if (readLen > 0)
    {
      /* update stored offset and length (non-streaming file) */
      if (fileType == WSF_EFS_FILE_TYPE_BULK)
      {
        wdxsCb.ftLen -= readLen;
        wdxsCb.ftOffset += readLen;
      }

      /* send notification */
      AttsHandleValueNtf(connId, WDXS_FTD_HDL, (uint16_t)readLen, buf);
      wdxsCb.txReadyMask &= ~(WDXS_TX_MASK_READY_BIT);
    }

    /* check if end of transfer reached */
    if (wdxsCb.ftLen == 0 || readLen == 0 || eof || wdxsCb.ftInProgress == WDXS_FTC_OP_ABORT)
    {
      wdxsCb.ftInProgress = WDXS_FTC_OP_NONE;
      wdxsCb.txReadyMask &= ~(WDXS_TX_MASK_FTD_BIT);
    }

    if (eof)
    {
      /* send EOF */
      wdxsFtcSendRsp(connId, WDXS_FTC_OP_EOF, wdxsCb.ftHandle, 0);
    }
  }
}
