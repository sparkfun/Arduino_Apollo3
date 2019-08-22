/*************************************************************************************************/
/*!
 *  \file   wdxs_stream.h
 *
 *  \brief  Wireless Data Exchange profile implementation - Stream Example.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2013-2017 ARM Ltd., all rights reserved.
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

#ifndef WDXS_STREAM_H
#define WDXS_STREAM_H

/**************************************************************************************************
  Constant Definitions
**************************************************************************************************/

/*! Type of waveform to output from the Example Stream */
#define WDXS_STREAM_WAVEFORM_SINE       0
#define WDXS_STREAM_WAVEFORM_STEP       1
#define WDXS_STREAM_WAVEFORM_SAWTOOTH   2

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     wdxsStreamInit
 *
 *  \brief  Example of creating a WDXS stream.
 *
 *  \param  none
 *
 *  \return None.
 */
/*************************************************************************************************/
void wdxsStreamInit(void);

/*************************************************************************************************/
/*!
 *  \fn     wdxsSetStreamWaveform
 *
 *  \brief  Changes the type of waveform transmitted by the stream.
 *
 *  \param  type - Identifier of the waveform
 *
 *  \return None.
 */
/*************************************************************************************************/
void wdxsSetStreamWaveform(uint8_t type);

#endif /* WDXS_STREAM_H */

