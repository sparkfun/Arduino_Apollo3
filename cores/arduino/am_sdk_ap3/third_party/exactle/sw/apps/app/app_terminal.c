/*************************************************************************************************/
/*!
 *  \file       app_terminal.c
 *
 *  \brief      Terminal handler.
 *
 *          $Date: 2016-03-29 14:55:12 -0700 (Tue, 29 Mar 2016) $
 *          $Revision: 6524 $
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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "terminal.h"
#include "app_ui.h"
#include "print.h"

#include "wsf_types.h"
#include "wsf_assert.h"
#include "wsf_trace.h"
#include "bstream.h"

#include "dm_api.h"

/**************************************************************************************************
  Local Function Prototypes
**************************************************************************************************/

/*! \brief    Button Command Handler */
static uint8_t appTerminalCommandBtnHandler(uint32_t argc, char **argv);

/*! \brief    Security Pin Code Command Handler. */
static uint8_t appTerminalPinCodeHandler(uint32_t argc, char **argv);

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! \brief    Button command. */
static terminalCommand_t appTerminalButtonPress = { NULL, "btn", "btn <ID> <s|m|l|x>", appTerminalCommandBtnHandler };

/*! \brief    Security Pin Code commands. */
static terminalCommand_t appTerminalPinCode = { NULL, "pin", "pin <ConnID> <Pin Code>", appTerminalPinCodeHandler };

/*************************************************************************************************/
/*!
 *  \fn     AppTerminalInit
 *
 *  \brief  Initialize terminal.
 *
 *  \param  handlerId   Handler ID for TerminalHandler().
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppTerminalInit(wsfHandlerId_t handlerId)
{
  TerminalRegisterCommand(&appTerminalButtonPress);
  TerminalRegisterCommand(&appTerminalPinCode);
}

/*************************************************************************************************/
/*!
 *  \fn     appTerminalCommandBtnHandler
 *
 *  \brief  Handler for a button press terminal command.
 *
 *  \param  argc      The number of arguments passed to the command.
 *  \param  argv      The array of arguments; the 0th argument is the command.
 *
 *  \return Error code.
 */
/*************************************************************************************************/
static uint8_t appTerminalCommandBtnHandler(uint32_t argc, char **argv)
{
  if (argc < 3)
  {
    return TERMINAL_ERROR_TOO_FEW_ARGUMENTS;
  }
  else if (argc == 3)
  {
    uint8_t btnIndx;
    uint8_t event;

    if (strcmp(argv[1], "1") == 0)
    {
      btnIndx = 1;
    }
    else if (strcmp(argv[1], "2") == 0)
    {
      btnIndx = 2;
    }
    else
    {
      return TERMINAL_ERROR_BAD_ARGUMENTS;
    }

    if (strcmp(argv[2], "d") == 0)
    {
      TerminalTxPrint("Button %s Press" TERMINAL_STRING_NEW_LINE, argv[1]);
      event = (btnIndx == 1? APP_UI_BTN_1_DOWN : APP_UI_BTN_2_DOWN);
    }
    else if (strcmp(argv[2], "s") == 0)
    {
      TerminalTxPrint("Short Button %s Press" TERMINAL_STRING_NEW_LINE, argv[1]);
      event = (btnIndx == 1? APP_UI_BTN_1_SHORT : APP_UI_BTN_2_SHORT);
    }
    else if (strcmp(argv[2], "m") == 0)
    {
      TerminalTxPrint("Medium Button %s Press" TERMINAL_STRING_NEW_LINE, argv[1]);
      event = (btnIndx == 1? APP_UI_BTN_1_MED : APP_UI_BTN_2_MED);
    }
    else if (strcmp(argv[2], "l") == 0)
    {
      TerminalTxPrint("Long Button %s Press" TERMINAL_STRING_NEW_LINE, argv[1]);
      event = (btnIndx == 1? APP_UI_BTN_1_LONG : APP_UI_BTN_2_LONG);
    }
    else if (strcmp(argv[2], "x") == 0)
    {
      TerminalTxPrint("Medium Button %s Press" TERMINAL_STRING_NEW_LINE, argv[1]);
      event = (btnIndx == 1? APP_UI_BTN_1_EX_LONG : APP_UI_BTN_2_EX_LONG);
    }
    else
    {
      return TERMINAL_ERROR_BAD_ARGUMENTS;
    }

    AppUiBtnTest(event);
  }
  else
  {
    return TERMINAL_ERROR_TOO_MANY_ARGUMENTS;
  }

  return TERMINAL_ERROR_OK;
}

/*************************************************************************************************/
/*!
 *  \fn     appTerminalPinCodeHandler
 *
 *  \brief  Handler for a pin code terminal command.
 *
 *  \param  argc      The number of arguments passed to the command.
 *  \param  argv      The array of arguments; the 0th argument is the command.
 *
 *  \return Error code.
 */
/*************************************************************************************************/
static uint8_t appTerminalPinCodeHandler(uint32_t argc, char **argv)
{
  if (argc < 2)
  {
    return TERMINAL_ERROR_TOO_FEW_ARGUMENTS;
  }
  else if (argc == 3)
  {
    uint32_t passkey;
    uint8_t  buf[SMP_PIN_LEN];
    uint8_t connId;

    passkey = atoi(argv[2]);
    connId = atoi(argv[1]);

    if (connId < 1 || connId > DM_CONN_MAX)
    {
      TerminalTxPrint("ConnID must be in the range [1 .. %d]\n", DM_CONN_MAX);
      return TERMINAL_ERROR_BAD_ARGUMENTS;
    }

    passkey %= 1000000;

    /* convert to byte buffer */
    buf[0] = UINT32_TO_BYTE0(passkey);
    buf[1] = UINT32_TO_BYTE1(passkey);
    buf[2] = UINT32_TO_BYTE2(passkey);

    /* send authentication response to DM */
    DmSecAuthRsp((dmConnId_t) connId, SMP_PIN_LEN, buf);
  }
  else
  {
    return TERMINAL_ERROR_TOO_MANY_ARGUMENTS;
  }

  return TERMINAL_ERROR_OK;
}
