/*************************************************************************************************/
/*!
 *  \file   crc32_notable.c
 *
 *  \brief  CRC-32 utilities.
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

#include "wsf_types.h"
#include "crc32.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Polynomial. */
#define CRC32_POLYNOMIAL_REV   UINT32_C(0xEDB88320)

/*************************************************************************************************/
/*!
 *  \fn     CalcCrc32
 *
 *  \brief  Calculate the CRC-32 of the given buffer.
 *
 *  \param  crcInit  Initial value of the CRC.
 *  \param  len      Length of the buffer.
 *  \param  pBuf     Buffer to compute the CRC.
 *
 *  \return None.
 */
/*************************************************************************************************/
uint32_t CalcCrc32(uint32_t crcInit, uint32_t len, uint8_t *pBuf)
{
  uint32_t crc = crcInit;
  unsigned int j;

  while (len > 0)
  {
    crc ^= *pBuf;

    for (j = 8; j > 0; j--)
    {
      if (crc & 1)
      {
        crc = crc >> 1 ^ CRC32_POLYNOMIAL_REV;
      }
      else
      {
        crc = crc >> 1;
      }
    }
    pBuf++;
    len--;
  }

  crc = crc ^ 0xFFFFFFFFU;

  return crc;
}
