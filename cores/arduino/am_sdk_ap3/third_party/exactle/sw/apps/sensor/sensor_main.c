/*************************************************************************************************/
/*!
 *  \file   sensor_main.c
 *
 *  \brief  Sensor sample application.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2015-2017 ARM Ltd., all rights reserved.
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
#include "app_api.h"
#include "app_db.h"
#include "app_ui.h"
#include "att_defs.h"
#include "att_api.h"
#include "bstream.h"
#include "dm_api.h"
#include "svc_core.h"
#include "svc_dis.h"
#include "wsf_msg.h"
#include "wsf_trace.h"

#include "sensor_api.h"
#include "gyro_api.h"
#include "svc_gyro.h"
#include "temp_api.h"
#include "svc_temp.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

#define SENSOR_MSG_START    0xA0

enum
{
  SENSOR_GYRO_TIMER_IND = SENSOR_MSG_START,
  SENSOR_TEMP_TIMER_IND
};

/*! Advertising timer timeout */
#define SENSOR_ADV_TIMEOUT_SEC          30000

/***************************************************************************************************
** Global Variables
***************************************************************************************************/

static wsfHandlerId_t sensorHandlerId;

/**************************************************************************************************
  Configurable Parameters
**************************************************************************************************/

/*! configurable parameters for slave */
static const appAdvCfg_t sensorAdvCfg =
{
  {SENSOR_ADV_TIMEOUT_SEC, 0, 0},         /*! Advertising durations in ms */
  {200,                    0, 0},         /*! Advertising intervals in 0.625 ms units */
};

/*! configurable parameters for slave */
static appSlaveCfg_t sensorSlaveCfg =
{
  1                                       /*! Maximum connections */
};

/*! configurable parameters for security */
static const appSecCfg_t sensorSecCfg =
{
  DM_AUTH_BOND_FLAG,                      /*! Authentication and bonding flags */
  0,                                      /*! Initiator key distribution flags */
  DM_KEY_DIST_LTK,                        /*! Responder key distribution flags */
  FALSE,                                  /*! TRUE if Out-of-band pairing data is present */
  FALSE                                   /*! TRUE to initiate security upon connection */
};

/*! configurable parameters for connection parameter update */
static const appUpdateCfg_t sensorUpdateCfg =
{
  0,                                      /*! Connection idle period in ms before attempting
                                             connection parameter update; set to zero to disable */
  640,                                    /*! Minimum connection interval in 1.25ms units */
  800,                                    /*! Maximum connection interval in 1.25ms units */
  0,                                      /*! Connection latency */
  900,                                    /*! Supervision timeout in 10ms units */
  5                                       /*! Number of update attempts before giving up */
};

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/* advertising data */
static uint8_t sensorAdvDataDisc[HCI_ADV_DATA_LEN]; /* value is set in sensorStart() */
static uint8_t sensorAdvDataConn[HCI_ADV_DATA_LEN]; /* value is set in sensorStart() */

static const uint8_t sensorAdvDataFlags[] =
{
  DM_FLAG_LE_LIMITED_DISC | DM_FLAG_LE_BREDR_NOT_SUP
};

/* scan data */
static uint8_t sensorScanDataDisc[HCI_ADV_DATA_LEN]; /* value is set in sensorStart() */
static uint8_t sensorScanDataConn[HCI_ADV_DATA_LEN]; /* value is set in sensorStart() */
static const char sensorScanDataLocalName[] =
{
  'S', 'e', 'n', 's', 'o', 'r'
};

/**************************************************************************************************
  CCC Data
**************************************************************************************************/

/* enumeration of client characteristic configuration descriptors */
enum
{
  APPS_MAIN_GYRO_DATA_CLIENT_CHR_CONFIG_CCC_IDX,
  APPS_MAIN_GYRO_TEMPDATA_CLIENT_CHR_CONFIG_CCC_IDX,
  APPS_MAIN_TEMP_DATA_CLIENT_CHR_CONFIG_CCC_IDX,
  APPS_MAIN_NUM_CCC_IDX
};

/* client characteristic configuration descriptors settings, indexed by above enumeration */
static const attsCccSet_t MainSensor_CccSet[APPS_MAIN_NUM_CCC_IDX] =
{
  /* cccd handle                           value range             security level */
  {GYRO_HANDLE_DATA_CLIENT_CHR_CONFIG,     ATT_CLIENT_CFG_NOTIFY,  DM_SEC_LEVEL_NONE},
  {GYRO_HANDLE_TEMPDATA_CLIENT_CHR_CONFIG, ATT_CLIENT_CFG_NOTIFY,  DM_SEC_LEVEL_NONE},
  {TEMP_HANDLE_DATA_CLIENT_CHR_CONFIG,     ATT_CLIENT_CFG_NOTIFY,  DM_SEC_LEVEL_NONE}
};

/*************************************************************************************************/
/*!
 *  \fn     sensorDmCback
 *
 *  \brief  Application DM callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void sensorDmCback(dmEvt_t *pDmEvt)
{
  dmEvt_t *pMsg;
  uint16_t  len;

  len = DmSizeOfEvt(pDmEvt);

  if ((pMsg = WsfMsgAlloc(len)) != NULL)
  {
    memcpy(pMsg, pDmEvt, len);
    WsfMsgSend(sensorHandlerId, pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     sensorAttCback
 *
 *  \brief  Application ATT callback.
 *
 *  \param  pAttEvt ATT callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void sensorAttCback(attEvt_t *pEvt)
{
  return;
}

/*************************************************************************************************/
/*!
 *  \fn     sensorCccCback
 *
 *  \brief  Application CCC callback.
 *
 *  \param  pEvt  CCC callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void sensorCccCback(attsCccEvt_t *pEvt)
{
  APP_TRACE_INFO3("ccc state ind value:%d handle:%d idx:%d", pEvt->value, pEvt->handle, pEvt->idx);

  switch (pEvt->idx)
  {
    case APPS_MAIN_GYRO_DATA_CLIENT_CHR_CONFIG_CCC_IDX:
    {
      if (pEvt->value == ATT_CLIENT_CFG_NOTIFY)
      {
        GyroMeasStart();
      }
      else
      {
        GyroMeasStop();
      }
      break;
    }
    case APPS_MAIN_TEMP_DATA_CLIENT_CHR_CONFIG_CCC_IDX:
    {
      if (pEvt->value == ATT_CLIENT_CFG_NOTIFY)
      {
        TempMeasStart();
      }
      else
      {
        TempMeasStop();
      }
      break;
    }
    default:
      break;
  }
}

/*************************************************************************************************/
/*!
 *  \fn     sensorSetSystemId
 *
 *  \brief  Set the system ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void sensorSetSystemId(void)
{
  uint8_t *bdaddr = HciGetBdAddr();
  uint8_t  sys_id[8];

  /* Formatted according to GATT specification for System ID characteristic (0x2A23). */
  sys_id[0] = bdaddr[0];
  sys_id[1] = bdaddr[1];
  sys_id[2] = bdaddr[2];
  sys_id[3] = 0xFE;
  sys_id[4] = 0xFF;
  sys_id[5] = bdaddr[3];
  sys_id[6] = bdaddr[4];
  sys_id[7] = bdaddr[5];
  AttsSetAttr(DIS_SID_HDL, sizeof(sys_id), sys_id);
}

/*************************************************************************************************/
/*!
 *  \fn     sensorSetup
 *
 *  \brief  Set up advertising and other procedures that need to be performed after
 *          device reset.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void sensorSetup(wsfMsgHdr_t *pMsg)
{
  /* set advertising response data for discoverable mode */
  memset(sensorAdvDataDisc, 0, sizeof(sensorAdvDataDisc));
  AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, 0, (uint8_t *) sensorAdvDataDisc);
  AppAdvSetAdValue(APP_ADV_DATA_DISCOVERABLE, DM_ADV_TYPE_FLAGS, sizeof(sensorAdvDataFlags),
                   (uint8_t *) sensorAdvDataFlags);

  /* set scan response data for discoverable mode */
  memset(sensorScanDataDisc, 0, sizeof(sensorScanDataDisc));
  AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, 0, (uint8_t *) sensorScanDataDisc);
  AppAdvSetAdValue(APP_SCAN_DATA_DISCOVERABLE, DM_ADV_TYPE_LOCAL_NAME, sizeof(sensorScanDataLocalName), (uint8_t *) sensorScanDataLocalName);

  /* set advertising response data for connectable mode */
  memset(sensorAdvDataConn, 0, sizeof(sensorAdvDataConn));
  AppAdvSetData(APP_ADV_DATA_CONNECTABLE, 0, (uint8_t *) sensorAdvDataConn);
  AppAdvSetAdValue(APP_ADV_DATA_CONNECTABLE, DM_ADV_TYPE_FLAGS, sizeof(sensorAdvDataFlags),
                   (uint8_t *) sensorAdvDataFlags);

  /* set scan response data for connectable mode */
  memset(sensorScanDataConn, 0, sizeof(sensorScanDataConn));
  AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, 0, (uint8_t *) sensorScanDataConn);
  AppAdvSetAdValue(APP_SCAN_DATA_CONNECTABLE, DM_ADV_TYPE_LOCAL_NAME, sizeof(sensorScanDataLocalName), (uint8_t *) sensorScanDataLocalName);

  /* set system ID according to BDADDR */
  sensorSetSystemId();

  /* start advertising */
  AppSetAdvType(DM_ADV_CONN_UNDIRECT);
  AppAdvStart(APP_MODE_AUTO_INIT);
}
/*************************************************************************************************/
/*!
 *  \fn     sensorBtnCback
 *
 *  \brief  Button press callback.
 *
 *  \param  btn    Button press.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void sensorBtnCback(uint8_t btn)
{
  dmConnId_t connId;

  /* button actions when connected */
  connId = AppConnIsOpen();
  if (connId != DM_CONN_ID_NONE)
  {
    switch (btn)
    {
      /* ignore button when connected */
      case APP_UI_BTN_1_SHORT:
        break;

      default:
        break;
    }
  }
  /* button actions when not connected */
  else
  {
    switch (btn)
    {
      case APP_UI_BTN_1_SHORT:
        /* start connectable advertising and reset timer */
        AppAdvStop();
        AppAdvStart(APP_MODE_CONNECTABLE);
        break;

      default:
        break;
    }
  }
}
/*************************************************************************************************/
/*!
 *  \fn     sesnorProcMsg
 *
 *  \brief  Process messages from the event handler.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void sensorProcMsg(wsfMsgHdr_t *pMsg)
{
  uint8_t uiEvent = APP_UI_NONE;

  switch (pMsg->event)
  {
    case ATT_MTU_UPDATE_IND:
      APP_TRACE_INFO1("Negotiated MTU %d", ((attEvt_t *)pMsg)->mtu);
      break;
    
    case DM_RESET_CMPL_IND:
    {
      sensorSetup(pMsg);
      uiEvent = APP_UI_RESET_CMPL;
      break;
    }

    case DM_CONN_OPEN_IND:
    {
      GyroMeasStart();
      TempMeasStart();
      uiEvent = APP_UI_CONN_OPEN;
      break;
    }

    case DM_CONN_CLOSE_IND:
    {
      GyroMeasStop();
      TempMeasStop();
      uiEvent = APP_UI_CONN_CLOSE;
      break;
    }

    case DM_ADV_START_IND:
    {
      uiEvent = APP_UI_ADV_START;
      break;
    }

    case DM_ADV_STOP_IND:
    {
      uiEvent = APP_UI_ADV_STOP;
      break;
    }

    case SENSOR_GYRO_TIMER_IND:
    {
      dmConnId_t connId = AppConnIsOpen();
      if (connId != DM_CONN_ID_NONE)
      {
        int16_t x, y, z;
        if (AppReadGyro(&x, &y, &z))
        {
          GyroMeasComplete(connId, x, y, z);
        }
      }
      break;
    }

    case SENSOR_TEMP_TIMER_IND:
    {
      dmConnId_t connId = AppConnIsOpen();
      if (connId != DM_CONN_ID_NONE)
      {
        int16_t temp;
        if (AppReadTemp(&temp))
        {
          TempMeasComplete(connId, temp);
        }
      }
      break;
    }

    default:
      break;
  }


  if (uiEvent != APP_UI_NONE)
  {
    AppUiAction(uiEvent);
  }
}

#ifndef APP_SENSOR_OVERRIDE
/*************************************************************************************************/
/*!
*  \fn     AppReadGyro
*
*  \brief  Read gyroscope sensor.
*
*  \param  pX        Storage for gyroscope x-axis reading.
*  \param  pY        Storage for gyroscope y-axis reading.
*  \param  pZ        Storage for gyroscope z-axis reading.
*
*  \return None.
*/
/*************************************************************************************************/
bool_t AppReadGyro(int16_t *pX, int16_t *pY, int16_t *pZ)
{
  static int16_t x = -100;
  static int16_t y = 0;
  static int16_t z = 100;
  *pX = x++;
  *pY = y++;
  *pZ = z++;

  return TRUE;
}

/*************************************************************************************************/
/*!
*  \fn     AppReadTemp
*
*  \brief  Read temperature sensor.
*
*  \param  pTemp     Storage for temperature reading.
*
*  \return None.
*/
/*************************************************************************************************/
bool_t AppReadTemp(int16_t *pTemp)
{
  static int16_t temp = 0;
  *pTemp = temp++;
  return TRUE;
}
#endif

/*************************************************************************************************/
/*!
 *  \fn     SensorHandlerInit
 *
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SensorHandlerInit(wsfHandlerId_t handlerId)
{
  /* store handler ID */
  sensorHandlerId = handlerId;

  /* Set configuration pointers */
  pAppAdvCfg    = (appAdvCfg_t *)&sensorAdvCfg;
  pAppSlaveCfg  = (appSlaveCfg_t *)&sensorSlaveCfg;
  pAppSecCfg    = (appSecCfg_t *)&sensorSecCfg;
  pAppUpdateCfg = (appUpdateCfg_t *)&sensorUpdateCfg;

  /* Initialize application framework */
  AppSlaveInit();
}

/*************************************************************************************************/
/*!
 *  \fn     SensorHandler
 *
 *  \brief  WSF event handler for application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SensorHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
  if (pMsg != NULL)
  {
    APP_TRACE_INFO1("sensor got evt %d", pMsg->event);

    if (pMsg->event >= DM_CBACK_START && pMsg->event <= DM_CBACK_END)
    {
      /* process advertising and connection-related messages */
      AppSlaveProcDmMsg((dmEvt_t *) pMsg);

      /* process security-related messages */
      AppSlaveSecProcDmMsg((dmEvt_t *) pMsg);
    }

    /* perform profile and user interface-related operations */
    sensorProcMsg(pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \fn     SensorStart
 *
 *  \brief  Start sensor application.
*
 *  \return None.
 */
/*************************************************************************************************/
void SensorStart(void)
{
  APP_TRACE_INFO0("sensor starting app");

  /* Register for stack callbacks */
  DmRegister(sensorDmCback);
  DmConnRegister(DM_CLIENT_ID_APP, sensorDmCback);
  AttRegister(sensorAttCback);
  AttConnRegister(AppServerConnCback);

  /* Register for app framework callbacks */
  AppUiBtnRegister(sensorBtnCback);

  /* Initialize attribute server database */
  SvcCoreAddGroup();
  SvcDisAddGroup();
  GyroStart(sensorHandlerId, SENSOR_GYRO_TIMER_IND);
  TempStart(sensorHandlerId, SENSOR_TEMP_TIMER_IND);

  /* set up CCCD table and callback */
  AttsCccRegister(APPS_MAIN_NUM_CCC_IDX, (attsCccSet_t *) MainSensor_CccSet, sensorCccCback);

  /* Reset the device */
  DmDevReset();
}

