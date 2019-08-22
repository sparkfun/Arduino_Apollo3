/*************************************************************************************************/
/*!
 *  \file   app_param.c
 *
 *  \brief  Application framework parameter database for BT4 external flash.
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

#include <string.h>
#include <stdio.h>
#include "wsf_types.h"
#include "wsf_assert.h"
#include "bstream.h"
#include "app_param.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/**************************************************************************************************
  Global Variables
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
void AppParamInit(void)
{

}

/*************************************************************************************************/
/*!
 *  \fn     AppParamClear()
 *
 *  \brief  Clear the parameter database.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppParamClear(void)
{

}


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
uint16_t AppParamWrite(uint16_t id, uint16_t valueLen, const uint8_t *pValue)
{
  FILE *pFile;
  char filename[32];
  uint16_t len = 0;

  sprintf(filename, "app_param_%d.dat", id);
  pFile = fopen(filename, "wb");

  if (pFile)
  {
    len = (uint16_t) fwrite(pValue, 1, valueLen, pFile);
    fclose(pFile);
  }

  return len;
}

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
uint16_t AppParamRead(uint16_t id, uint16_t valueLen, uint8_t *pValue)
{
  FILE *pFile;
  char filename[32];
  uint16_t len = 0;

  sprintf(filename, "app_param_%d.dat", id);
  pFile = fopen(filename, "rb");

  if (pFile)
  {
    len = (uint16_t) fread(pValue, 1, valueLen, pFile);
    fclose(pFile);
  }

  return len;
}
