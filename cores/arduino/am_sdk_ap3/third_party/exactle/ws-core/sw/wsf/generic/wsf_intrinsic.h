/*************************************************************************************************/
/*!
 *  \file   wsf_intrinsic.h
 *
 *  \brief  CPU core intrinsic functions.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2009-2017 ARM Ltd., all rights reserved.
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
#ifndef WSF_INTRINSIC_H
#define WSF_INTRINSIC_H

#ifdef __arm__
#include "device_cm3.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \def    WsfLeadingMsbZeros
 *
 *  \brief  Count leading MSB zero.
 *
 *  \param  value   Value to count leading zeros.
 *
 *  \return Number of leading zeros.
 */
/*************************************************************************************************/
#ifdef __arm__
#define WsfClzMsb(v)    __CLZ(v)
#else
static inline uint8_t WsfClzMsb(uint32_t value)
{
  /* Stub for platform specific implementation. */
  return 0;
}
#endif

/*************************************************************************************************/
/*!
 *  \def    WsfLeadingMsbOnes
 *
 *  \brief  Count leading MSB ones.
 *
 *  \param  value   Value to count leading ones.
 *
 *  \return Number of leading ones.
 */
/*************************************************************************************************/
#ifdef __arm__
#define WsfCloMsb(v)    __CLZ(~v)
#else
static inline uint8_t WsfCloMsb(uint32_t value)
{
  return WsfClzMsb(~value);
}
#endif

/*************************************************************************************************/
/*!
 *  \def    WsfLeadingLsbZeros
 *
 *  \brief  Count leading LSB zero.
 *
 *  \param  value   Value to count leading zeros.
 *
 *  \return Number of leading zeros.
 */
/*************************************************************************************************/
#ifdef __arm__
#define WsfClzLsb(v)    __CLZ(__REV(v))
#else
static inline uint8_t WsfClzLsb(uint32_t value)
{
  /* Stub for platform specific implementation. */
  return 0;
}
#endif

/*************************************************************************************************/
/*!
 *  \def    WsfLeadingLsbOnes
 *
 *  \brief  Count leading LSB ones.
 *
 *  \param  value   Value to count leading ones.
 *
 *  \return Number of leading ones.
 */
/*************************************************************************************************/
#ifdef __arm__
#define WsfCloLsb(v)    __CLZ(__REV(~v))
#else
static inline uint8_t WsfCloLsb(uint32_t value)
{
  return WsfClzLsb(~value);
}
#endif

#ifdef __cplusplus
};
#endif

#endif /* WSF_INTRINSIC_H */
