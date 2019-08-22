/*************************************************************************************************/
/*!
 *  \file   wdxs_main.c
 *
 *  \brief  Wireless Data Exchange profile implementation.
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
#include "wsf_types.h"
#include "wstr.h"
#include "wsf_trace.h"
#include "wsf_assert.h"
#include "wsf_efs.h"
#include "wsf_os.h"
#include "sec_api.h"
#include "bstream.h"
#include "svc_wdxs.h"
#include "wdxs_api.h"
#include "wdxs_main.h"
#include "dm_api.h"
#include "att_api.h"
#include "app_api.h"

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! application control block */
wdxsCb_t wdxsCb;

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! RAM File Media Configuration */
#define WDXS_RAM_LOCATION         ((uint32_t)WdxsRamBlock)
#define WDXS_RAM_SIZE             (WDXS_FLIST_MAX_LEN + WDXS_APP_RAM_MEDIA_SIZE)
#define WDXS_RAM_END              (WDXS_RAM_LOCATION + WDXS_RAM_SIZE)

/**************************************************************************************************
  Local Function Prototypes
**************************************************************************************************/
static uint8_t WdxsRamErase(uint32_t address, uint32_t size);
static uint8_t WdxsRamRead(uint8_t *pBuf, uint32_t address, uint32_t size);
static uint8_t WdxsRamWrite(const uint8_t *pBuf, uint32_t address, uint32_t size);

/**************************************************************************************************
  Function Prototypes
**************************************************************************************************/
void WdxsAuSecComplete(secAes_t *pAes);

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! Reserve RAM for use by the RAM EFS Media */
static uint8_t WdxsRamBlock[WDXS_RAM_SIZE];

/*! EFS RAM Media Control Block */
static const wsfEfsMedia_t WDXS_RamMediaCtrl =
{
  WDXS_RAM_LOCATION,
  WDXS_RAM_LOCATION + WDXS_RAM_SIZE,
  1,
  NULL,
  WdxsRamErase,
  WdxsRamRead,
  WdxsRamWrite,
  NULL
};

/*************************************************************************************************/
/*!
 *  \fn     WdxsRamErase
 *
 *  \brief  Erase function for the EFS RAM media.
 *
 *  \return none.
 *
 */
/*************************************************************************************************/
static uint8_t WdxsRamErase(uint32_t address, uint32_t size)
{
  uint8_t *pMem = (uint8_t *) address;
  memset(pMem, 0xFF, size);
  return TRUE;
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsRamRead
 *
 *  \brief  Read function for the EFS RAM media.
 *
 *  \return none.
 *
 */
/*************************************************************************************************/
static uint8_t WdxsRamRead(uint8_t *pBuf, uint32_t address, uint32_t size)
{
  uint8_t *pMem = (uint8_t *) address;
  memcpy(pBuf, pMem, size);
  return TRUE;
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsRamWrite
 *
 *  \brief  Write function for the EFS RAM media.
 *
 *  \return none.
 *
 */
/*************************************************************************************************/
static uint8_t WdxsRamWrite(const uint8_t *pBuf, uint32_t address, uint32_t size)
{
  uint8_t *pMem = (uint8_t *) address;
  memcpy(pMem, pBuf, size);
  return TRUE;
}

/*************************************************************************************************/
/*!
 *  \fn     wsfFormatFileResource
 *
 *  \brief  Format file list information for the given file.
 *
 *  \return none.
 */
/*************************************************************************************************/
static void wdxsFormatFileResource(uint8_t *pData, wsfEfsHandle_t handle)
{
  UINT16_TO_BSTREAM(pData, handle);
  UINT8_TO_BSTREAM(pData, WsfEfsGetFileType(handle));
  UINT8_TO_BSTREAM(pData, WsfEfsGetFilePermissions(handle) & WSF_EFS_REMOTE_PERMISSIONS_MASK);
  UINT32_TO_BSTREAM(pData, WsfEfsGetFileSize(handle));
  WstrnCpy((char *)pData, WsfEfsGetFileName(handle), WSF_EFS_NAME_LEN);
  WstrnCpy((char *)pData+WSF_EFS_NAME_LEN, WsfEfsGetFileVersion(handle), WSF_EFS_VERSION_LEN);
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsUpdateListing
 *
 *  \brief  Create the file list.
 *
 *  \return none.
 */
/*************************************************************************************************/
void WdxsUpdateListing(void)
{
  uint8_t *pTmp;
  uint8_t header[WDXS_FLIST_HDR_SIZE];
  uint8_t record[WDXS_FLIST_RECORD_SIZE];
  uint32_t position = 0, totalSize = 0;
  uint32_t fileCount = 0;
  uint8_t i;

  position = WDXS_FLIST_HDR_SIZE;

  for (i=0; i<WSF_EFS_MAX_FILES; i++)
  {
    if (WsfEfsGetFileByHandle(i) && (WsfEfsGetFilePermissions(i) & WSF_EFS_REMOTE_VISIBLE))
    {
      /* Update the total size and file count */
      totalSize += WsfEfsGetFileSize(i);
      fileCount++;

      wdxsFormatFileResource(record, i);

      /* Write the record */
      WsfEfsPut(WDXS_FLIST_HANDLE, position, record, WDXS_FLIST_RECORD_SIZE);
      position += WDXS_FLIST_RECORD_SIZE;
    }
  }

  /* Add the header after calculating the total_size and file_count */
  pTmp = header;
  UINT8_TO_BSTREAM(pTmp, WDXS_FLIST_FORMAT_VER);
  UINT16_TO_BSTREAM(pTmp, fileCount);
  UINT32_TO_BSTREAM(pTmp, totalSize);

  /* Write the header */
  WsfEfsPut(WDXS_FLIST_HANDLE, 0, header, WDXS_FLIST_HDR_SIZE);
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsSetCccIdx
 *
 *  \brief  Set the CCCD index used by the application for WDXS service characteristics.
 *
 *  \param  dcCccIdx   Device Control CCCD index.
 *  \param  auCccIdx   Authentication CCCD index.
 *  \param  ftcCccIdx  File Transfer Control CCCD index.
 *  \param  ftdCccIdx  File Transfer Data CCCD index.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxsSetCccIdx(uint8_t dcCccIdx, uint8_t auCccIdx, uint8_t ftcCccIdx, uint8_t ftdCccIdx)
{
  wdxsCb.dcCccIdx = dcCccIdx;
  wdxsCb.auCccIdx = auCccIdx;
  wdxsCb.ftcCccIdx = ftcCccIdx;
  wdxsCb.ftdCccIdx = ftdCccIdx;
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsWriteCback
 *
 *  \brief  ATTS write callback for proprietary service.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t wdxsWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                       uint16_t offset, uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
  uint8_t status;

#if WDXS_AU_ENABLED == TRUE

  /* Require peer authentication before writing to any characteristic
    (except for the authentication characteristic) */
  if ((wdxsAuCb.reqAuthLevel != WDXS_AU_LVL_NONE) && (handle != WDXS_AU_HDL))
  {
    if ((wdxsAuCb.authState != WDXS_AU_STATE_AUTHORIZED) || (wdxsAuCb.authLevel < wdxsAuCb.reqAuthLevel))
    {
      APP_TRACE_INFO1("WDXS: WriteCback unauthorized state=%d", wdxsAuCb.authState);
      return WDXS_APP_AUTH_REQUIRED;
    }
  }

#endif /* WDXS_AU_ENABLED */

  switch (handle)
  {
#if WDXS_DC_ENABLED == TRUE
    /* Device configuration */
    case WDXS_DC_HDL:
      status = wdxsDcWrite(connId, len, pValue);
      break;
#endif /* WDXS_DC_ENABLED */

    /* File transfer control */
    case WDXS_FTC_HDL:
      status = wdxsFtcWrite(connId, len, pValue);
      break;

    /* File transfer data */
    case WDXS_FTD_HDL:
      status = wdxsFtdWrite(connId, len, pValue);
      break;

#if WDXS_AU_ENABLED == TRUE
    /* Authentication */
    case WDXS_AU_HDL:
      status = wdxsAuWrite(connId, len, pValue);
      break;
#endif /* WDXS_AU_ENABLED */

    default:
      APP_TRACE_INFO1("WDXS: WriteCback unexpected handle=%d", handle);
      status = ATT_ERR_HANDLE;
      break;
  }


  return status;
}

/*************************************************************************************************/
/*!
 *  \fn     wdxsProcTxPath
 *
 *  \brief  Process TX data path
 *
 *  \return None.
 */
/*************************************************************************************************/
static void wdxsProcTxPath(void)
{
  dmConnId_t  connId;

  /* Check for a connection */
  if ((connId = AppConnIsOpen()) != DM_CONN_ID_NONE)
  {
    /* Check if ready to transmit a message */
    if (wdxsCb.txReadyMask & WDXS_TX_MASK_READY_BIT)
    {

#if WDXS_DC_ENABLED == TRUE
      /* Device configuration */
      if (wdxsCb.txReadyMask & WDXS_TX_MASK_DC_BIT)
      {
        wdxsDcSend(connId);
        return;
      }
#endif /* WDXS_DC_ENABLED */

      /* File Transfer Control */
      if (wdxsCb.txReadyMask & WDXS_TX_MASK_FTC_BIT)
      {
        wdxsFtcSend(connId);
        return;
      }

#if WDXS_AU_ENABLED == TRUE
      /* Authentication */
      if (wdxsCb.txReadyMask & WDXS_TX_MASK_AU_BIT)
      {
        wdxsAuSend(connId);
        return;
      }
#endif /* WDXS_AU_ENABLED */

      /* File Transfer Data */
      if (wdxsCb.txReadyMask & WDXS_TX_MASK_FTD_BIT)
      {
        wdxsFtdSend(connId);
        return;
      }
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsHandler
 *
 *  \brief  WSF event handler for application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxsHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
  APP_TRACE_INFO1("WDXS: Task Handler Evt=%d", event);

  if (event & WDXS_EVT_TX_PATH)
  {
    wdxsProcTxPath();
  }

#if WDXS_AU_ENABLED == TRUE

  if (event & WDXS_EVT_AU_SEC_COMPLETE)
  {
    WdxsAuSecComplete((secAes_t*) pMsg);
  }

#endif /* WDXS_AU_ENABLED */
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsProcDmMsg
 *
 *  \brief  Called by application to notify the WDXS of DM Events.
 *
 *  \param  pEvt   Pointer to the DM Event
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxsProcDmMsg(dmEvt_t *pEvt)
{
  switch (pEvt->hdr.event)
  {
    case DM_CONN_CLOSE_IND:
      if (wdxsDcCb.doReset)
      {
        WdxsResetSystem();
      }
      break;

    case DM_CONN_OPEN_IND:
      /* Initialize connection parameters */
      wdxsCb.txReadyMask = WDXS_TX_MASK_READY_BIT;
      wdxsCb.ftInProgress = WDXS_FTC_OP_NONE;
      wdxsCb.ftLen = 0;
      wdxsCb.ftOffset = 0;
#if WDXS_AU_ENABLED == TRUE
      wdxsAuCb.authLevel = WDXS_AU_LVL_NONE;
      wdxsAuCb.authState = WDXS_AU_STATE_UNAUTHORIZED;
#endif /* WDXS_AU_ENABLED */
      wdxsCb.connInterval = pEvt->connOpen.connInterval;
      wdxsCb.connLatency = pEvt->connOpen.connLatency;
      wdxsCb.supTimeout = pEvt->connOpen.supTimeout;
      break;

    case DM_CONN_UPDATE_IND:
      if (pEvt->hdr.status == HCI_SUCCESS)
      {
        wdxsCb.connInterval = pEvt->connUpdate.connInterval;
        wdxsCb.connLatency = pEvt->connUpdate.connLatency;
        wdxsCb.supTimeout = pEvt->connUpdate.supTimeout;
      }
      wdxsDcUpdateConnParam((dmConnId_t) pEvt->hdr.param, pEvt->hdr.status);
      break;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsAttCback
 *
 *  \brief  Called by application to notify the WDXS of ATT Events.
 *
 *  \param  pEvt   Pointer to the ATT Event
 *
 *  \return None.
 */
/*************************************************************************************************/
uint8_t WdxsAttCback(attEvt_t *pEvt)
{
  if (pEvt->handle < WDXS_START_HDL || pEvt->handle > WDXS_END_HDL)
  {
    return FALSE;
  }

  APP_TRACE_INFO2("WDXS: AttHook handle=%d event=%d", pEvt->handle, pEvt->hdr.event);

  /* trigger tx data path on confirm */
  if (pEvt->hdr.event == ATTS_HANDLE_VALUE_CNF &&
      pEvt->hdr.status == ATT_SUCCESS)
  {
    wdxsCb.txReadyMask |= WDXS_TX_MASK_READY_BIT;
    WsfSetEvent(wdxsCb.handlerId, WDXS_EVT_TX_PATH);
  }

  return TRUE;
}

/*************************************************************************************************/
/*!
 *  \fn     WdxsHandlerInit
 *
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WdxsHandlerInit(wsfHandlerId_t handlerId)
{
  wsfEsfAttributes_t attr;

  APP_TRACE_INFO0("WDXS: WdxsHandlerInit");

  /* Initialize the control block */
  memset(&wdxsCb, 0, sizeof(wdxsCb));
  wdxsCb.txReadyMask = WDXS_TX_MASK_READY_BIT;

  /* Store Handler ID */
  wdxsCb.handlerId = handlerId;

  /* Register the WDXS Service */
  SvcWdxsRegister(wdxsWriteCback);
  SvcWdxsAddGroup();

  /* Initialize the embedded file system */
  WsfEfsInit();

  /* Register the RAM Media */
  memset(WdxsRamBlock, 0xFF, sizeof(WdxsRamBlock));
  WsfEfsRegisterMedia(&WDXS_RamMediaCtrl, WDXS_RAM_MEDIA);

  /* Set attributes for the WDXS File List */
  attr.type = WSF_EFS_FILE_TYPE_BULK;
  attr.permissions = WSF_EFS_LOCAL_PUT_PERMITTED | WSF_EFS_REMOTE_GET_PERMITTED;
  WstrnCpy(attr.name, "Listing", WSF_EFS_NAME_LEN);
  WstrnCpy(attr.version, "1.0", WSF_EFS_VERSION_LEN);

  /* Create a file in RAM to contain the list WDXS File List */
  WsfEfsAddFile(WDXS_FLIST_MAX_LEN, WDXS_RAM_MEDIA, &attr, WSF_EFS_FILE_OFFSET_ANY);
}
