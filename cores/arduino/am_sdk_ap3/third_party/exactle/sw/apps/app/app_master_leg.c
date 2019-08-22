/*************************************************************************************************/
/*!
 *  \file   app_master.c
 *
 *  \brief  Application framework module for legacy master.  This module can be used with both
 *          DM legacy and extended scanning and connect.
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

#include "wsf_types.h"
#include "wsf_trace.h"
#include "dm_api.h"
#include "app_api.h"
#include "app_main.h"

/*************************************************************************************************/
/*!
 *  \fn     appMasterScanMode
 *
 *  \brief  Check if current scanning mode is legacy scanning.
 *
 *  \return TRUE if legacy scanning mode. FALSE, otherwise.
 */
/*************************************************************************************************/
static bool_t appMasterScanMode(void)
{
  /* legacy master app works with both DM legacy and extended scanning */

  /* if first time since last power-on or reset */
  if (appMasterCb.scanMode == APP_SCAN_MODE_NONE)
  {
    /* set scanning mode to legacy */
    appMasterCb.scanMode = APP_SCAN_MODE_LEG;

    return TRUE;
  }

  if (appMasterCb.scanMode == APP_SCAN_MODE_LEG)
  {
    return TRUE;
  }

  APP_TRACE_WARN0("Invalid DM scanning mode; mode configured as extended");

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     AppScanStart
 *        
 *  \brief  Start scanning.   A scan is performed using the given discoverability mode,
 *          scan type, and duration.
 *
 *  \param  mode      Discoverability mode.
 *  \param  scanType  Scan type.
 *  \param  duration  The scan duration, in milliseconds.  If set to zero, scanning will
 *                    continue until AppScanStop() is called.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppScanStart(uint8_t mode, uint8_t scanType, uint16_t duration)
{
  if (appMasterScanMode())
  {
    DmScanSetInterval(HCI_SCAN_PHY_LE_1M_BIT, &pAppMasterCfg->scanInterval, &pAppMasterCfg->scanWindow);

    DmScanStart(HCI_SCAN_PHY_LE_1M_BIT, mode, &scanType, TRUE, duration, 0);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppScanStop
 *        
 *  \brief  Stop scanning.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppScanStop(void)
{
  if (appMasterScanMode())
  {
    /* stop address resolution */
    appMasterCb.inProgress = FALSE;

    DmScanStop();
  }
}

/*************************************************************************************************/
/*!
 *  \fn     AppConnOpen
 *        
 *  \brief  Open a connection to a peer device with the given address.
 *
 *  \param  addrType  Address type.
 *  \param  pAddr     Peer device address.
 *  \param  dbHdl     Device database handle.
 *
 *  \return Connection identifier.
 */
/*************************************************************************************************/
dmConnId_t AppConnOpen(uint8_t addrType, uint8_t *pAddr, appDbHdl_t dbHdl)
{
  if (appMasterScanMode())
  {
    return appConnOpen(HCI_INIT_PHY_LE_1M_BIT, addrType, pAddr, dbHdl);
  }

  /* wrong connect mode */
  return DM_CONN_ID_NONE;
}
