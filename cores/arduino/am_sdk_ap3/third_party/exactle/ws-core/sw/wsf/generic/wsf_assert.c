/*************************************************************************************************/
/*!
 *  \file   wsf_assert.c
 *
 *  \brief  Assert implementation.
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

#include "wsf_types.h"
#include "wsf_assert.h"
#include "wsf_trace.h"

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! \brief      Assert control block. */
static struct
{
  uint16_t numAssert;
  bool_t   disAssertTrap;
} wsfAssertCb = { 0 };

/*************************************************************************************************/
/*!
 *  \def    WsfAssert
 *
 *  \brief  Perform an assert action.
 *
 *  \param  pFile   Name of file originating assert.
 *  \param  line    Line number of assert statement.
 *
 *  \return None.
 */
/*************************************************************************************************/
#if WSF_TOKEN_ENABLED == TRUE
void WsfAssert(uint16_t modId, uint16_t line)
#else
void WsfAssert(const char *pFile, uint16_t line)
#endif
{
  wsfAssertCb.numAssert++;

#if WSF_TOKEN_ENABLED == TRUE
  WSF_TRACE_ERR2("Assertion detected on %s:%u", modId, line);
#else
  WSF_TRACE_ERR2("Assertion detected on %s:%u", pFile, line);
#endif

  /* spin forever if fatal error occurred */
  for (;;)
  {
    volatile bool_t escape = wsfAssertCb.disAssertTrap;

    /*
     *  However, you can exit with a debugger by setting variable 'escape'.
     *  Handy to see where the assert happened if you cannot view the call stack.
     */
    if (escape)
    {
      break;
    }
  }
}

/*************************************************************************************************/
/*!
 *  \def    WsfAssertNum
 *
 *  \brief  Get number of asserts.
 *
 *  \return Number of asserts.
 */
/*************************************************************************************************/
uint16_t WsfAssertNum(void)
{
  return wsfAssertCb.numAssert;
}

/*************************************************************************************************/
/*!
 *  \def    WsfAssertTrapEnable
 *
 *  \brief  Enable assert trap.
 *
 *  \param  enaAssertTrap     TRUE to enable assert trap.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WsfAssertTrapEnable(bool_t enaAssertTrap)
{
  wsfAssertCb.disAssertTrap = !enaAssertTrap;
}
