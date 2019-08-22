/*************************************************************************************************/
/*!
 *  \file   hid_main.c
 *
 *  \brief  Human Interface Device Profile.
 *
 *          $Date: 2017-02-07 19:05:49 -0600 (Tue, 07 Feb 2017) $
 *          $Revision: 11110 $
 *
 *  Copyright (c) 2015-2016-2017 ARM Ltd., all rights reserved.
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
#include "wsf_assert.h"
#include "wsf_trace.h"
#include "att_api.h"
#include "svc_hid.h"
#include "app_api.h"
#include "hid_api.h"

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! HID control block */
typedef struct
{
  const hidConfig_t         *pConfig;             /* HID Configuration passed in from the application */
} hidCb_t;

hidCb_t hidCb;

/*************************************************************************************************/
/*!
 *  \fn     hidGetReportHandle
 *        
 *  \brief  Gets the attribute handle of a report given the type and id
 *
 *  \param  type    The type of report (HID_REPORT_TYPE_INPUT, HID_REPORT_TYPE_OUTPUT, HID_REPORT_TYPE_FEATURE)
 *  \param  id      The ID of the report
 *
 *  \return The attribute handle for the report or ATT_HANDLE_NONE if the report is not in the map.
 */
/*************************************************************************************************/
static uint16_t hidGetReportHandle(uint8_t type, uint8_t id)
{
  hidReportIdMap_t *pMap; 
  uint8_t count;
  int8_t i;

  WSF_ASSERT(hidCb.pConfig);
  WSF_ASSERT(hidCb.pConfig->pReportIdMap);

  pMap = hidCb.pConfig->pReportIdMap;
  count = hidCb.pConfig->reportIdMapSize;

  for (i = 0; i < count; i++)
  {
    if (pMap[i].type == type && pMap[i].id == id)
    {
      return pMap[i].handle;
    }
  }

  return ATT_HANDLE_NONE;
}

/*************************************************************************************************/
/*!
 *  \fn     hidGetReportIdMap
 *        
 *  \brief  Gets the type and ID of a report given a handle
 *
 *  \param  handle      The attribute handle
 *
 *  \return A pointer to the hidReportIdMap_t with the type and ID
 */
/*************************************************************************************************/
static hidReportIdMap_t *hidGetReportIdMap(uint16_t handle)
{
  hidReportIdMap_t *pMap; 
  uint8_t count;
  int8_t i;

  WSF_ASSERT(hidCb.pConfig);
  WSF_ASSERT(hidCb.pConfig->pReportIdMap);

  pMap = hidCb.pConfig->pReportIdMap;
  count = hidCb.pConfig->reportIdMapSize;

  for (i = 0; i < count; i++)
  {
    if (pMap[i].handle == handle)
    {
      return &pMap[i];
    }
  }

  return NULL;
}

/*************************************************************************************************/
/*!
 *  \fn     HidGetControlPoint
 *        
 *  \brief  Gets the HID control point value.
 *
 *  \param  None.
 *
 *  \return The control point value (HID_CONTROL_POINT_SUSPEND or HID_CONTROL_POINT_RESUME).
 */
/*************************************************************************************************/
uint8_t HidGetControlPoint()
{
  uint16_t len = 1;
  uint8_t *pValue;

  AttsGetAttr(HID_CONTROL_POINT_HDL, &len, &pValue);

  return *pValue;
}

/*************************************************************************************************/
/*!
 *  \fn     HidGetProtocolMode
 *        
 *  \brief  Gets the HID protocol mode value.
 *
 *  \param  None.
 *
 *  \return The protocol mode value (HID_PROTOCOL_MODE_REPORT or HID_PROTOCOL_MODE_BOOT).
 */
/*************************************************************************************************/
uint8_t HidGetProtocolMode()
{
  uint16_t len = 1;
  uint8_t *pValue;

  AttsGetAttr(HID_PROTOCOL_MODE_HDL, &len, &pValue);

  return *pValue;
}

/*************************************************************************************************/
/*!
 *  \fn     HidSetProtocolMode
 *        
 *  \brief  Sets the HID protocol mode for keyboard and mouse devices that support Boot Mode.
 *
 *  \param  protocolMode    The protocol mode (HID_PROTOCOL_MODE_REPORT or HID_PROTOCOL_MODE_BOOT)
 *
 *  \return None.
 */
/*************************************************************************************************/
void HidSetProtocolMode(uint8_t protocolMode)
{
  AttsSetAttr(HID_PROTOCOL_MODE_HDL, 1, &protocolMode);
}

/*************************************************************************************************/
/*!
 *  \fn     HidSendInputReport
 *        
 *  \brief  Sends an input report to the host
 *
 *  \param  connId      The connection ID
 *  \param  reportId    The Report ID
 *  \param  len         The length of the report in bytes
 *  \param  pValue      A buffer containing the report
 *
 *  \return none.
 */
/*************************************************************************************************/
void HidSendInputReport(dmConnId_t connId, uint8_t reportId, uint16_t len, uint8_t *pValue)
{
  uint16_t handle = hidGetReportHandle(HID_REPORT_TYPE_INPUT, reportId);

  if (handle != ATT_HANDLE_NONE)
  {
    /* Store the attribute value */
    AttsSetAttr(handle, len, pValue);

    /* send notification */
    AttsHandleValueNtf(connId, handle, len, pValue);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     HidAttsWriteCback
 *        
 *  \brief  Called on an ATTS Write to the HID Service.
 *
 *  \return ATT status.
 *
 */
/*************************************************************************************************/
uint8_t HidAttsWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                          uint16_t offset, uint16_t len, uint8_t *pValue,
                          attsAttr_t *pAttr)
{
  hidReportIdMap_t *pIdMap;

  WSF_ASSERT(hidCb.pConfig);

  switch (handle)
  {
  case HID_CONTROL_POINT_HDL:

    /* notify the application */
    if (hidCb.pConfig->infoCback != NULL)
    {
      hidCb.pConfig->infoCback(connId, HID_INFO_CONTROL_POINT, *pValue);
    }
    break;

  case HID_PROTOCOL_MODE_HDL:

    /* Record the value of the protocol mode */
    HidSetProtocolMode(pValue[0]);
    
    if (hidCb.pConfig->infoCback != NULL)
    {
      hidCb.pConfig->infoCback(connId, HID_INFO_PROTOCOL_MODE, *pValue);
    }
    break;

  case HID_KEYBOARD_BOOT_OUT_HDL:

    /* set the attribute value so it can be read by the host */
    AttsSetAttr(handle, len, pValue);

    /* notify the application */
    if (hidCb.pConfig->outputCback != NULL)
    {
      pIdMap = hidGetReportIdMap(handle);
    
      if (pIdMap != NULL)
      {
        hidCb.pConfig->outputCback(connId, pIdMap->id, len, pValue);
      }
    }
    break;

  default:

    pIdMap = hidGetReportIdMap(handle);

    if (pIdMap != NULL)
    {
      /* set the attribute value so it can be read by the host */
      AttsSetAttr(handle, len, pValue);

      /* notify the application */
      if (pIdMap->type == HID_REPORT_TYPE_FEATURE)
      {
        if (hidCb.pConfig->featureCback != NULL)
        {
          hidCb.pConfig->featureCback(connId, pIdMap->id, len, pValue);
        }
      }
      else if (pIdMap->type == HID_REPORT_TYPE_OUTPUT)
      {
        if (hidCb.pConfig->outputCback != NULL)
        {
          hidCb.pConfig->outputCback(connId, pIdMap->id, len, pValue);
        }
      }
    }
    break;
  }

  return 0;
}

/*************************************************************************************************/
/*!
 *  \fn     HidInit
 *        
 *  \brief  Initialize the HID profile.
 *
 *  \param  pConfig     HID Configuration structure
 *
 *  \return None.
 */
/*************************************************************************************************/
void HidInit(const hidConfig_t *pConfig)
{
  WSF_ASSERT(pConfig);

  /* Store the configuration */
  hidCb.pConfig = pConfig;
}
