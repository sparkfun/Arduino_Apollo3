/*************************************************************************************************/
/*!
 *  \file   wdxc_ft.c
 *
 *  \brief  Wireless Data Exchange profile client - File Transfer.
 *
 *          $Date: 2017-06-06 14:23:06 -0500 (Tue, 06 Jun 2017) $
 *          $Revision: 12300 $
 *
 *  Copyright (c) 2017 ARM Ltd., all rights reserved.
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
#include "wdxc_api.h"
#include "wdxc_main.h"
#include "dm_api.h"
#include "app_api.h"

/**************************************************************************************************
  External Variables
**************************************************************************************************/
extern wdxcCb_t wdxcCb;

/*************************************************************************************************/
/*!
 *  \fn     WdxcFtcSendAbort
 *
 *  \brief  Send a request to abort a wdx operation.
 *
 *  \param  connId          Connection ID.
 *  \param  fileHdl         Handle of file to abort operation on peer device
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxcFtcSendAbort(dmConnId_t connId, uint16_t fileHdl)
{
  uint8_t   buf[WDXS_FTC_ABORT_LEN];
  uint8_t   *p = buf;
  uint16_t  handle = wdxcCb.conn[connId-1].pHdlList[WDXC_FTC_HDL_IDX];

  UINT8_TO_BSTREAM(p, WDXS_FTC_OP_ABORT);
  UINT16_TO_BSTREAM(p, fileHdl);

  AttcWriteReq(connId, handle, WDXS_FTC_ABORT_LEN, buf);
}

/*************************************************************************************************/
/*!
 *  \fn     WdxcFtcSendEraseFile
 *
 *  \brief  Send a request to erase a file.
 *
 *  \param  connId          Connection ID.
 *  \param  fileHdl         Handle of file to erase on peer device
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxcFtcSendEraseFile(dmConnId_t connId, uint16_t fileHdl)
{
  uint8_t   buf[WDXS_FTC_ERASE_LEN];
  uint8_t   *p = buf;
  uint16_t  handle = wdxcCb.conn[connId-1].pHdlList[WDXC_FTC_HDL_IDX];

  UINT8_TO_BSTREAM(p, WDXS_FTC_OP_ERASE_REQ);
  UINT16_TO_BSTREAM(p, fileHdl);

  AttcWriteReq(connId, handle, WDXS_FTC_ERASE_LEN, buf);
}

/*************************************************************************************************/
/*!
 *  \fn     WdxcFtcSendVerifyFile
 *
 *  \brief  Send a request to verify a file.
 *
 *  \param  connId          Connection ID.
 *  \param  fileHdl         Handle of file to verify on peer device
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxcFtcSendVerifyFile(dmConnId_t connId, uint16_t fileHdl)
{
  uint8_t   buf[WDXS_FTC_VERIFY_LEN];
  uint8_t   *p = buf;
  uint16_t  handle = wdxcCb.conn[connId-1].pHdlList[WDXC_FTC_HDL_IDX];

  UINT8_TO_BSTREAM(p, WDXS_FTC_OP_VERIFY_REQ);
  UINT16_TO_BSTREAM(p, fileHdl);

  AttcWriteReq(connId, handle, WDXS_FTC_VERIFY_LEN, buf);
}

/*************************************************************************************************/
/*!
 *  \fn     WdxcFtcSendPutReq
 *
 *  \brief  Send a request to put a block of data into a file on the peer device
 *
 *  \param  connId          Connection ID.
 *  \param  fileHdl         Handle of file on peer device
 *  \param  offset          The offset from the beginning of the file in bytes
 *  \param  len             The number of bytes to put
 *  \param  fileSize        The size of the file in bytes
 *  \param  type            reserved
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxcFtcSendPutReq(dmConnId_t connId, uint16_t fileHdl, uint32_t offset,
                       uint32_t len, uint32_t fileSize, uint8_t type)
{
  uint8_t   buf[WDXS_FTC_PUT_LEN];
  uint8_t   *p = buf;
  uint16_t  handle = wdxcCb.conn[connId-1].pHdlList[WDXC_FTC_HDL_IDX];

  UINT8_TO_BSTREAM(p, WDXS_FTC_OP_PUT_REQ);
  UINT16_TO_BSTREAM(p, fileHdl);

  UINT32_TO_BSTREAM(p, offset);
  UINT32_TO_BSTREAM(p, len);
  UINT32_TO_BSTREAM(p, fileSize);
  UINT8_TO_BSTREAM(p, type);

  AttcWriteReq(connId, handle, WDXS_FTC_PUT_LEN, buf);
}

/*************************************************************************************************/
/*!
 *  \fn     WdxcFtcSendGetReq
 *
 *  \brief  Send a request to perform a get a block of data from a file on the peer device
 *
 *  \param  connId          Connection ID.
 *  \param  fileHdl         Handle of file to verify on peer device
 *  \param  offset          The offset from the beginning of the file in bytes
 *  \param  len             The number of bytes to get
 *  \param  type            reserved
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxcFtcSendGetReq(dmConnId_t connId, uint16_t fileHdl, uint32_t offset, uint32_t len, uint8_t type)
{
  uint8_t   buf[WDXS_FTC_GET_LEN];
  uint8_t   *p = buf;
  uint16_t  handle = wdxcCb.conn[connId-1].pHdlList[WDXC_FTC_HDL_IDX];

  UINT8_TO_BSTREAM(p, WDXS_FTC_OP_GET_REQ);
  UINT16_TO_BSTREAM(p, fileHdl);

  UINT32_TO_BSTREAM(p, offset);
  UINT32_TO_BSTREAM(p, len);
  UINT8_TO_BSTREAM(p, type);

  AttcWriteReq(connId, handle, WDXS_FTC_GET_LEN, buf);
}

/*************************************************************************************************/
/*!
 *  \fn     WdxcFtdSendBlock
 *
 *  \brief  Send a data block to the peer device
 *
 *  \param  connId          Connection ID.
 *  \param  len             Size of pData in bytes
 *  \param  pData           Data to put to the file
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxcFtdSendBlock(dmConnId_t connId, uint32_t len, uint8_t *pData)
{
  uint16_t  handle = wdxcCb.conn[connId-1].pHdlList[WDXC_FTD_HDL_IDX];

  AttcWriteReq(connId, handle, (uint16_t) len, pData);
}
