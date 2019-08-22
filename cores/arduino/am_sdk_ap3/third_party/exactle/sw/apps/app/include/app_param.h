/*************************************************************************************************/
/*!
 *  \file   app_param.h
 *
 *  \brief  Application framework parameter database.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2015-2017 ARM Ltd., all rights reserved.
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
#ifndef APP_PARAM_H
#define APP_PARAM_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     AppParamInit()
 *
 *  \brief  Initialize the parameter database.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppParamInit(void);

/*************************************************************************************************/
/*!
 *  \fn     AppParamClear()
 *
 *  \brief  Clear the parameter database.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppParamClear(void);

/*************************************************************************************************/
/*!
 *  \fn     AppParamWrite
 *
 *  \brief  Write parameter value.
 *
 *  \param  id          Identifier.
 *  \param  valueLen    Value length in bytes.
 *  \param  pValue      Value data.
 *
 *  \return Number of bytes written.
 */
/*************************************************************************************************/
uint16_t AppParamWrite(uint16_t id, uint16_t valueLen, const uint8_t *pValue);

/*************************************************************************************************/
/*!
 *  \fn     AppParamRead
 *
 *  \brief  Read parameter value.
 *
 *  \param  id          Identifier.
 *  \param  valueLen    Maximum value length in bytes.
 *  \param  pValue      Storage value data.
 *
 *  \return Number of bytes read.
 */
/*************************************************************************************************/
uint16_t AppParamRead(uint16_t id, uint16_t valueLen, uint8_t *pValue);

#ifdef __cplusplus
};
#endif

#endif /* APP_PARAM_H */
