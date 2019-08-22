/*************************************************************************************************/
/*!
 *  \file   utils.h
 *
 *  \brief  Utility functions.
 *
 *          $Date: 2016-03-29 14:55:12 -0700 (Tue, 29 Mar 2016) $
 *          $Revision: 6524 $
 *
 *  Copyright (c) 2015 ARM, Ltd., all rights reserved.
 *  ARM confidential and proprietary.
 *
 *  IMPORTANT.  Your use of this file is governed by a Software License Agreement
 *  ("Agreement") that must be accepted in order to download or otherwise receive a
 *  copy of this file.  You may not use or copy this file for any purpose other than
 *  as described in the Agreement.  If you do not agree to all of the terms of the
 *  Agreement do not use this file and delete all copies in your possession or control;
 *  if you do not have a copy of the Agreement, you must contact ARM, Ltd. prior
 *  to any use, copying or further distribution of this software.
 */
/*************************************************************************************************/

#ifndef __UTILS_H
#define __UTILS_H

/***************************************************************************************************
** INCLUDES
***************************************************************************************************/

#include <stdarg.h>

#include "wsf_types.h"

#include "am_util.h"

#if defined(__GNUC__) || defined(__CC_ARM)
#define PRINTF_ATTRIBUTE(a, b) __attribute__((format(printf, a, b)))
#else
#define PRINTF_ATTRIBUTE(a, b)
#endif

/***************************************************************************************************
** DEFINES
***************************************************************************************************/

#define UTIL_MAC_ADDR_LEN 6

/*------------------------------------------------------------------------------------------------*/

#define UTIL_IS_DIGIT(c)        ((c >= '0') && (c <= '9'))
#define UTIL_IS_XDIGIT(c)      (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || \
                                ((c >= 'A') && (c <= 'F')))

/*------------------------------------------------------------------------------------------------*/

#define UTIL_DIGIT_TO_INT(c)   (((c >= '0') && (c <= '9')) ? (uint8_t)(c - '0') : 0u)
#define UTIL_XDIGIT_TO_INT(c)  (((c >= '0') && (c <= '9')) ? (uint8_t)(c - '0') : \
                                ((c >= 'a') && (c <= 'f')) ? (uint8_t)(c - 'a' + 10u) : \
                                ((c >= 'A') && (c <= 'F')) ? (uint8_t)(c - 'A' + 10u) : 0u)

/***************************************************************************************************
** FUNCTIONS
***************************************************************************************************/
#define Util_VSNPrintf(s, size, format, ap) am_util_stdio_vsprintf(s, format, ap)
#define Util_SNPrintf(s, size, ...) am_util_stdio_vsprintf(s, __VA_ARGS__)

/*--------------------------------------------------------------------------------------------------
** Util_ParseMacAddr()
**
** DESCRIPTION: Parse MAC address from string.
**
** PARAMETERS:  s     Pointer to string to parse
**              addr  Pointer to buffer that will receive MAC address
**
** RETURNS:     Number of characters consumed from string
**------------------------------------------------------------------------------------------------*/
//int32_t Util_ParseMacAddr(const char *s, uint8_t (*addr)[UTIL_MAC_ADDR_LEN]);

/*--------------------------------------------------------------------------------------------------
** Util_ParseUInt()
**
** DESCRIPTION: Parse unsigned integer from string.
**
** PARAMETERS:  s     Pointer to string to parse
**              u     Pointer to variable that will receive integer
**              base  Base of integer (between 2 and 36, inclusive) or 0, for automatic detection
**
** RETURNS:     Number of characters consumed from string
**------------------------------------------------------------------------------------------------*/
//int32_t Util_ParseUInt(const char *s, uint32_t *u, uint32_t base);

#endif /* __UTILS_H */
