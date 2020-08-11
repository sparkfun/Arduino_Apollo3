/* Author: @justiceamoh and Nathan Seidle
  Created: November 19th, 2019

  This example demonstrates how to read audio data and output
  it to a WAV file. This sketch outputs raw serial; an accompanying
  python script visualizes and coverts the raw data to a WAV file.

  Note: Audio samples are generated fast enough that we need to output
  serial at 500kbps.

  The PDM hardware is setup to take a sample every 64us (15.625kHz sample rate)
  The PDM library uses DMA to transfer 4096 bytes every 262ms and stores the
  data between two internal buffers. So check available() often and call getData
  to prevent buffer overruns.
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <PDM.h> //Include PDM library included with the Aruino_Apollo3 core
AP3_PDM myPDM;   //Create instance of PDM class with our buffer

#define pdmDataSize 4096 //Library requires array be 4096
uint16_t pdmData[pdmDataSize];

// -----------------
// PDM Configuration
// -----------------
void *PDMHandle = NULL;
am_hal_pdm_config_t newConfig = {
    .eClkDivider = AM_HAL_PDM_MCLKDIV_1,
    .eLeftGain = AM_HAL_PDM_GAIN_0DB,
    .eRightGain = AM_HAL_PDM_GAIN_P90DB, //Found empirically
    .ui32DecimationRate = 48,            // OSR = 1500/16 = 96 = 2*SINCRATE --> SINC_RATE = 48
    .bHighPassEnable = 0,
    .ui32HighPassCutoff = 0xB,
    .ePDMClkSpeed = AM_HAL_PDM_CLK_1_5MHZ,
    .bInvertI2SBCLK = 0,
    .ePDMClkSource = AM_HAL_PDM_INTERNAL_CLK,
    .bPDMSampleDelay = 0,
    .bDataPacking = 1,
    .ePCMChannels = AM_HAL_PDM_CHANNEL_RIGHT,
    .ui32GainChangeDelay = 1,
    .bI2SEnable = 0,
    .bSoftMute = 0,
    .bLRSwap = 0,
};

void setup()
{
  Serial.begin(500000);
  delay(10);

  if (myPDM.begin() == false) // Turn on PDM with default settings, start interrupts
  {
    Serial.println("PDM Init failed. Are you sure these pins are PDM capable?");
    while (1)
      ;
  }
  myPDM.updateConfig(newConfig); //Send config struct
}

void loop()
{
  if (myPDM.available())
  {
    myPDM.getData(pdmData, pdmDataSize);

    //Print data to serial port
    Serial.write((uint8_t *)pdmData, sizeof(pdmData));
  }
}