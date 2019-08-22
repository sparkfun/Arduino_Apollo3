/*************************************************************************************************/
/*!
 *  \file   app_slave_ae.c
 *
 *  \brief  Application framework module for extended slave.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2016-2017 ARM Ltd., all rights reserved.
 *  ARM confidential and proprietary.
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
#include "dm_api.h"
#include "app_api.h"
#include "app_main.h"

/*************************************************************************************************/
/*!
 *  \fn     appSlaveExtAdvStart
 *
 *  \brief  Utility function to start extended advertising.
 *
 *  \param  advHandle    Advertising handle.
 *  \param  cfgAdvParam  Whether to configure advertising parameters
 *
 *  \return None.
 */
/*************************************************************************************************/
static void appSlaveExtAdvStart(uint8_t advHandle, bool_t setAdvParam)
{
  appAdvStart(1, &advHandle, pAppExtAdvCfg->advInterval, pAppExtAdvCfg->advDuration,
              pAppExtAdvCfg->maxEaEvents, setAdvParam);
}

/*************************************************************************************************/
/*!
 *  \fn     appSlaveExtAdvTypeChanged
 *
 *  \brief  Utility function to handle change in advertising type.
 *
 *  \param  pMsg      Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void appSlaveExtAdvTypeChanged(dmEvt_t *pMsg)
{
  /* clear advertising type changed flag */
  appSlaveCb.advTypeChanged[pMsg->advSetStop.advHandle] = FALSE;

  /* set advertising state */
  appSlaveCb.advState[pMsg->advSetStop.advHandle] = APP_ADV_STATE1;

  /* restart advertising */
  appSlaveExtAdvStart(pMsg->advSetStop.advHandle, TRUE);
}

/*************************************************************************************************/
/*!
 *  \fn     appSlaveNextExtAdvState
 *
 *  \brief  Set the next advertising state.
 *
 *  \param  pMsg       Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void appSlaveNextExtAdvState(dmEvt_t *pMsg)
{
  /* if adv hasn't been stopped and all adv/scan data haven't been sent */
  if ((appSlaveCb.advState[pMsg->advSetStop.advHandle] != APP_ADV_STOPPED) &&
      !appSlaveCb.advDataSynced[pMsg->advSetStop.advHandle])
  {
    /* set advertising state */
    appSlaveCb.advState[pMsg->advSetStop.advHandle] = APP_ADV_STATE1;

    /* restart advertising with rest of adv/scan data */
    appSlaveExtAdvStart(pMsg->advSetStop.advHandle, FALSE);

    return;
  }

  /* done with this advertising set */
  appSlaveCb.advState[pMsg->advSetStop.advHandle] = APP_ADV_STOPPED;
}

/*************************************************************************************************/
/*!
 *  \fn     appSlaveExtAdvStop
 *
 *  \brief  Handle a DM_ADV_SET_STOP_IND event.
 *
 *  \param  pMsg      Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void appSlaveExtAdvStop(dmEvt_t *pMsg)
{
  /* if advertising set was terminated */
  if (pMsg->hdr.event == DM_ADV_SET_STOP_IND)
  {
    /* if advertising was stopped for change to advertising type */
    if (appSlaveCb.advTypeChanged[pMsg->advSetStop.advHandle])
    {
      appSlaveExtAdvTypeChanged(pMsg);
    }
    /* else if advertising successfully ended with connection being created */
    else if (pMsg->hdr.status == HCI_SUCCESS)
    {
      /* advertising is stopped once a connection is opened */
      appSlaveCb.advState[pMsg->advSetStop.advHandle] = APP_ADV_STOPPED;
    }
    /* else advertising ended for other reasons */
    else
    {
      appSlaveNextExtAdvState(pMsg);
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     appSlaveExtAdvMode
 *
 *  \brief  Check if current advertising mode is extended advertising.
 *
 *  \return TRUE if extended advertising mode. FALSE, otherwise.
 */
/*************************************************************************************************/
static bool_t appSlaveExtAdvMode(void)
{
  uint8_t i;

  /* if DM extended advertising */
  if (DmAdvModeExt())
  {
    /* if first time since last power-on or reset */
    if (appSlaveCb.advStopCback == NULL)
    {
      appSlaveCb.advStopCback = appSlaveExtAdvStop;
      appSlaveCb.advRestartCback = NULL;

      /* for each advertising set */
      for (i = 0; i < DM_NUM_ADV_SETS; i++)
      {
        /* configure whether to use legacy advertising PDUs */
        DmAdvUseLegacyPdu(i, pAppExtAdvCfg->useLegacyPdu[i]);
      }

      return TRUE;
    }
  }

  if (appSlaveCb.advStopCback == appSlaveExtAdvStop)
  {
    return TRUE;
  }

  APP_TRACE_WARN0("Invalid DM advertising mode; mode configured as legacy");

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppExtAdvSetData
 *
 *  \brief  Set extended advertising data.
 * 
 *  \param  advHandle Advertising handle.
 *  \param  location  Data location.
 *  \param  len       Length of the data.  Maximum length is 31 bytes if advertising set uses
 *                    legacy advertising PDUs with extended advertising.
 *  \param  pData     Pointer to the data.
 *  \param  bufLen    Length of the data buffer maintained by Application.  Minimum length is
 *                    31 bytes.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppExtAdvSetData(uint8_t advHandle, uint8_t location, uint16_t len, uint8_t *pData, uint16_t bufLen)
{
  uint16_t maxLen;

  WSF_ASSERT(advHandle < DM_NUM_ADV_SETS);

  if (appSlaveExtAdvMode())
  {
    /* if advertsing set uses legacy advertising PDUs with extended advertising */
    if (pAppExtAdvCfg->useLegacyPdu[advHandle])
    {
      /* maximum advertising data length supported by Controller is 31 bytes */
      maxLen = HCI_ADV_DATA_LEN;

      /* legacy advertising data length cannot exceed 31 bytes */
      if (len > HCI_ADV_DATA_LEN)
      {
        len = HCI_ADV_DATA_LEN;
      }
    }
    else
    {
      /* get maximum advertising data length supported by Controller */
      maxLen = HciGetMaxAdvDataLen();
    }

    appAdvSetData(advHandle, location, len, pData, bufLen, maxLen);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppExtAdvStart
 *
 *  \brief  Start extended advertising using the parameters for the given advertising set and mode.
 *
 *  \param  numSets     Number of advertising sets.
 *  \param  pAdvHandles Advertising handles array.
 *  \param  mode        Discoverable/connectable mode.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppExtAdvStart(uint8_t numSets, uint8_t *pAdvHandles, uint8_t mode)
{
  uint8_t  i;
  uint16_t advInterval[DM_NUM_ADV_SETS];
  uint16_t advDuration[DM_NUM_ADV_SETS];
  uint8_t  maxEaEvents[DM_NUM_ADV_SETS];

  WSF_ASSERT(numSets <= DM_NUM_ADV_SETS);

  if (appSlaveExtAdvMode())
  {
    for (i = 0; i < numSets; i++)
    {
      WSF_ASSERT(pAdvHandles[i] < DM_NUM_ADV_SETS);

      /* initialize advertising state */
      appSlaveCb.advState[pAdvHandles[i]] = APP_ADV_STATE1;

      if (appSlaveCb.discMode != APP_MODE_NONE)
      {
        /* start advertising from beginning of advertising data */
        appSlaveResetAdvDataOffset(pAdvHandles[i]);
      }

      /* build advertising parameters */
      advInterval[i] = pAppExtAdvCfg->advInterval[pAdvHandles[i]];
      advDuration[i] = pAppExtAdvCfg->advDuration[pAdvHandles[i]];
      maxEaEvents[i] = pAppExtAdvCfg->maxEaEvents[pAdvHandles[i]];
    }

    appSlaveAdvStart(numSets, pAdvHandles, advInterval, advDuration, maxEaEvents, TRUE, mode);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppExtAdvStop
 *
 *  \brief  Stop extended advertising.  If the number of sets is set to 0 then all advertising
 *          sets are disabled.
 *
 *  \param  numSets   Number of advertising sets.
 *  \param  advHandle Advertising handle array.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppExtAdvStop(uint8_t numSets, uint8_t *pAdvHandles)
{
  WSF_ASSERT(numSets <= DM_NUM_ADV_SETS);

  if (appSlaveExtAdvMode())
  {
    appAdvStop(numSets, pAdvHandles);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppExtAdvSetAdValue
 *
 *  \brief  Set the value of an advertising data element in the extended advertising or scan
 *          response data.  If the element already exists in the data then it is replaced
 *          with the new value.  If the element does not exist in the data it is appended
 *          to it, space permitting.
 *
 *          There is special handling for the device name (AD type DM_ADV_TYPE_LOCAL_NAME).
 *          If the name can only fit in the data if it is shortened, the name is shortened
 *          and the AD type is changed to DM_ADV_TYPE_SHORT_NAME.
 *
 *  \param  advHandle Advertising handle.
 *  \param  location  Data location.
 *  \param  adType    Advertising data element type.
 *  \param  len       Length of the value.  Maximum length is 31 bytes if advertising set uses
 *                    legacy advertising PDUs with extended advertising.
 *  \param  pValue    Pointer to the value.
 *
 *  \return TRUE if the element was successfully added to the data, FALSE otherwise.
 */
/*************************************************************************************************/
bool_t AppExtAdvSetAdValue(uint8_t advHandle, uint8_t location, uint8_t adType, uint8_t len,
                           uint8_t *pValue)
{
  WSF_ASSERT(advHandle < DM_NUM_ADV_SETS);

  if (appSlaveExtAdvMode())
  {
    return appAdvSetAdValue(advHandle, location, adType, len, pValue);
  }

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppExtSetAdvType
 *
 *  \brief  Set extended advertising type.
 *
 *  \param  advHandle Advertising handle.
 *  \param  advType   Advertising type.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppExtSetAdvType(uint8_t advHandle, uint8_t advType)
{
  WSF_ASSERT(advHandle < DM_NUM_ADV_SETS);

  if (appSlaveExtAdvMode())
  {
    appSetAdvType(advHandle, advType, pAppExtAdvCfg->advInterval[advHandle],
                  pAppExtAdvCfg->advDuration[advHandle], pAppExtAdvCfg->maxEaEvents[advHandle], TRUE);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppExtConnAccept
 *
 *  \brief  Accept a connection to a peer device with the given address using a given advertising
 *          set.
 *
 *  \param  advHandle Advertising handle.
 *  \param  advType   Advertising type.
 *  \param  addrType  Address type.
 *  \param  pAddr     Peer device address.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppExtConnAccept(uint8_t advHandle, uint8_t advType, uint8_t addrType, uint8_t *pAddr)
{
  WSF_ASSERT(advHandle < DM_NUM_ADV_SETS);

  if (appSlaveExtAdvMode())
  {
    /* start connectable directed advertising */
    appConnAccept(advHandle, advType, pAppExtAdvCfg->advInterval[advHandle],
                  pAppExtAdvCfg->advDuration[advHandle], pAppExtAdvCfg->maxEaEvents[advHandle],
                  addrType, pAddr);
  }
}
