/*************************************************************************************************/
/*!
 *  \file   udsc_api.h
 *
 *  \brief  User Data Service Collector.
 *
 *          $Date: 2017-03-14 13:38:00 -0500 (Tue, 14 Mar 2017) $
 *          $Revision: 11545 $
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
#ifndef UDSC_API_H
#define UDSC_API_H

#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! UDSC service enumeration of handle indexes of characteristics to be discovered */
enum
{
  UDSC_DBCI_HDL_IDX,               /*! Database Change Interval */
  UDSC_DCBI_CCC_HDL_IDX,           /*! Database Change Interval CCC descriptor */
  UDSC_UI_HDL_IDX,                 /*! User Index */
  UDSC_UCP_IDX,                    /*! User Control Point */
  UDSC_UCP_CCC_HDL_IDX,            /*! User Control Point CCC descriptor */
  UDSC_HDL_LIST_LEN                /*! Handle list length */
};

/*! User Control Point Opcodes */
#define UDSC_UCP_OPCODE_RNU              0x01    /* Register New User */
#define UDSC_UCP_OPCODE_CONSENT          0x02    /* Consent */
#define UDSC_UCP_OPCODE_DUD              0x03    /* Delete User Data */
#define UDSC_UCP_OPCODE_RESPONSE         0x20    /* Command Response */

/*! User Control Point Response Values */
#define UDSC_UCP_RSP_SUCCESS             0x01    /* Success */
#define UDSC_UCP_RSP_OP_NOT_SUPPORTED    0x02    /* Opcode not supported */
#define UDSC_UCP_RSP_INVALID_PARAMETER   0x03    /* Invalid Parameter */
#define UDSC_UCP_RSP_OP_FAILED           0x04    /* Operation Failed */
#define UDSC_UCP_RSP_NOT_AUTHORIZED      0x05    /* User Not Authorized */


/**************************************************************************************************
  Callback Function Datatypes
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     UdsRspCback_t
 *
 *  \brief  UDS Control Point Response Callback.
 *
 *  \param  connId      Connection ID.
 *  \param  opcode      Cmd opcode being responded to.
 *  \param  response    Response code.
 *  \param  index       USer index (only set when opcode is UDSC_UCP_OPCODE_RNU)
 *
 *  \return None
 */
/*************************************************************************************************/
typedef void (*UdsRspCback_t)(dmConnId_t connId, uint8_t opcode, uint8_t response, uint8_t index);

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     UdscDiscover
 *        
 *  \brief  Perform service and characteristic discovery for User Data service.  Parameter 
 *          pHdlList must point to an array of length UDSC_HDL_LIST_LEN.  If discovery is 
 *          successful the handles of discovered characteristics and descriptors will be set
 *          in pHdlList.
 *
 *  \param  connId    Connection identifier.
 *  \param  pHdlList  Characteristic handle list.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscDiscover(dmConnId_t connId, uint16_t *pHdlList);

/*************************************************************************************************/
/*!
 *  \fn     UdscValueUpdate
 *        
 *  \brief  Process a value received in an ATT read response, notification, or indication 
 *          message.  Parameter pHdlList must point to an array of length UDSC_PLXS_HDL_LIST_LEN. 
 *          If the ATT handle of the message matches a handle in the handle list the value
 *          is processed, otherwise it is ignored.
 *
 *  \param  pHdlList  Characteristic handle list.
 *  \param  pMsg      ATT callback message.
 *
 *  \return ATT_SUCCESS if handle is found, ATT_ERR_NOT_FOUND otherwise.
 */
/*************************************************************************************************/
uint8_t UdscValueUpdate(uint16_t *pHdlList, attEvt_t *pMsg);

/*************************************************************************************************/
/*!
 *  \fn     UdscReadUserIndex
 *        
 *  \brief  Read the user index characteristic.
 *
 *  \param  connId      Connection identifier.
 *  \param  handle      Attribute handle. 
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscReadUserIndex(dmConnId_t connId, uint16_t handle);

/*************************************************************************************************/
/*!
 *  \fn     UdscReadDatabaseChangeIncrement
 *        
 *  \brief  Read the database change increment characteristic.
 *
 *  \param  connId      Connection identifier.
 *  \param  handle      Attribute handle. 
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscReadDatabaseChangeIncrement(dmConnId_t connId, uint16_t handle);

/*************************************************************************************************/
/*!
 *  \fn     UdscWriteDatabaseChangeIncrement
 *        
 *  \brief  Write the database change increment characteristic.
 *
 *  \param  connId      Connection identifier.
 *  \param  handle      Attribute handle. 
 *  \param  increment   DB Change Increment
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscWriteDatabaseChangeIncrement(dmConnId_t connId, uint16_t handle, uint32_t increment);

/*************************************************************************************************/
/*!
 *  \fn     UdscRegisterNewUser
 *        
 *  \brief  Write to the user control point characteristic - Register New User.
 *
 *  \param  connId        Connection identifier.
 *  \param  handle        Attribute handle. 
 *  \param  consentCode   Consent code (0-9999)
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscRegisterNewUser(dmConnId_t connId, uint16_t handle, uint16_t consentCode);

/*************************************************************************************************/
/*!
 *  \fn     UdscConsent
 *        
 *  \brief  Write to the user control point characteristic - Consent.
 *
 *  \param  connId        Connection identifier.
 *  \param  handle        Attribute handle. 
 *  \param  index         User Index
 *  \param  consentCode   Consent code (0-9999 - provided when user was registered)
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscConsent(dmConnId_t connId, uint16_t handle, uint8_t index, uint16_t consentCode);

/*************************************************************************************************/
/*!
 *  \fn     UdscDeleteUserData
 *        
 *  \brief  Write to the user control point characteristic - Delete User Data.
 *
 *  \param  connId        Connection identifier.
 *  \param  handle        Attribute handle. 
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscDeleteUserData(dmConnId_t connId, uint16_t handle);

/*************************************************************************************************/
/*!
 *  \fn     UdscClose
 *        
 *  \brief  Called by the application when a connection closes.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscClose();

/*************************************************************************************************/
/*!
 *  \fn     UdscInit
 *        
 *  \brief  Initialize User Data Service collector callbacks.
 *
 *  \param  handlerId      Application task handler ID. 
 *  \param  timerEvent     Application timer event for control point timeout. 
 *  \param  rspCback       Callback to receive control point response messages. 
 *
 *  \return None.
 */
/*************************************************************************************************/
void UdscInit(wsfHandlerId_t handlerId, uint8_t timerEvent, UdsRspCback_t rspCback);

#ifdef __cplusplus
};
#endif

#endif /* UDSC_API_H */
