/*************************************************************************************************/
/*!
 *  \file   wdxc_stream.c
 *
 *  \brief  Wireless Data Exchange profile client - Stream utility functions.
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
#include "wsf_types.h"
#include "wstr.h"
#include "wsf_trace.h"
#include "wsf_assert.h"
#include "wsf_efs.h"
#include "bstream.h"
#include "dm_api.h"
#include "att_api.h"
#include "app_api.h"
#include "wdxc_api.h"
#include "wdxc_main.h"
#include "wdxs_defs.h"

/**************************************************************************************************
  External Variables
**************************************************************************************************/
extern wdxcCb_t wdxcCb;

/*************************************************************************************************/
/*!
 *  \fn     WdxcStreamStart
 *
 *  \brief  Send a request to start a stream of a given file handle on the given connection.
 *
 *  \param  connId    Connection ID.
 *  \param  fileHdl   Handle of the file.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxcStreamStart(dmConnId_t connId, uint16_t fileHdl)
{
  wdxcConnCb_t *pConnCb = &wdxcCb.conn[connId - 1];

  /* Verify WDXC is Idle */
  if (pConnCb->fileHdl == WSF_EFS_INVALID_HANDLE)
  {
    pConnCb->fileHdl = fileHdl;

    /* Send a get request with len set the MTU length (offset is ignored for streams) */
    WdxcFtcSendGetReq(connId, fileHdl, 0, AttGetMtu(connId), 0);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     WdxcStreamStop
 *
 *  \brief  Stop the active stream.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxcStreamStop(dmConnId_t connId)
{
  wdxcConnCb_t *pConnCb = &wdxcCb.conn[connId - 1];

  /* Send an abort to stop the stream */
  WdxcFtcSendAbort(connId, pConnCb->fileHdl);

  /* Set file handle to invalid to indicate no file operation is in progress */
  pConnCb->fileHdl = WSF_EFS_INVALID_HANDLE;
}
