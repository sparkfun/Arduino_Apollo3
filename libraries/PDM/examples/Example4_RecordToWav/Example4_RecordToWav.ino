/* Author: @justiceamoh and Nathan Seidle
  Created: November 19th, 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to read audio data and output
  it to a WAV file. This sketch outputs raw serial; an accompanying
  python script visualizes and converts the raw data to a WAV file.

  Note: Wave files are 1024kbps which is higher than most audio players
  can handle. Use VLC to play the output files.

  Note: Audio samples are generated fast enough that we need to output
  serial at 500kbps.
*/

#include "am_bsp.h"

// ----------------
// Global Variables
// ----------------
#define BUFFSIZE 512
uint32_t PDMDataBuffer[BUFFSIZE];
int16_t *pi16Buffer = (int16_t *)PDMDataBuffer;

//Rather than a cicular buffer we record to one buffer while the other is printed to serial
volatile int16_t outBuffer1[BUFFSIZE];
volatile int16_t outBuffer2[BUFFSIZE];
volatile int buff1New = false;
volatile int buff2New = false;

am_hal_pdm_transfer_t sTransfer;

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

// -----------------
// PDM Configuration
// -----------------
void initPDM(void)
{
  // Initialize, power-up and configure PDM
  am_hal_pdm_initialize(0, &PDMHandle);
  am_hal_pdm_power_control(PDMHandle, AM_HAL_PDM_POWER_ON, false);
  am_hal_pdm_configure(PDMHandle, &newConfig);
  am_hal_pdm_enable(PDMHandle);

  // Configure PDM pins
  am_hal_gpio_pinconfig(AM_BSP_PDM_DATA, g_AM_BSP_PDM_DATA);
  am_hal_gpio_pinconfig(AM_BSP_PDM_CLOCK, g_AM_BSP_PDM_CLOCK);

  // Configure PDM interrupts - set to trigger on DMA completion
  am_hal_pdm_interrupt_enable(PDMHandle, (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));
  // Configure DMA and target address.
  sTransfer.ui32TargetAddr = (uint32_t)PDMDataBuffer;
  sTransfer.ui32TotalCount = BUFFSIZE * 2;

  // Start the data transfer.
  am_hal_pdm_enable(PDMHandle);
  am_util_delay_ms(100);

  am_hal_pdm_fifo_flush(PDMHandle);
  am_hal_pdm_dma_start(PDMHandle, &sTransfer);

  // Enable PDM interrupt
  NVIC_EnableIRQ(PDM_IRQn);
}

// -----------------------------
// PDM Interrupt Service Routine
// -----------------------------
extern "C" void am_pdm_isr(void)
{
  uint32_t ui32Status;

  // Read the interrupt status.
  am_hal_pdm_interrupt_status_get(PDMHandle, &ui32Status, true);
  am_hal_pdm_interrupt_clear(PDMHandle, ui32Status);

  // Once DMA transaction completes, move to Queue & Start next conversion
  if (ui32Status & AM_HAL_PDM_INT_DCMP)
  {

    //Store in the first available buffer
    if (buff1New == false)
    {
      for (int i = 0; i < BUFFSIZE; i++)
      {
        outBuffer1[i] = pi16Buffer[i];
      }
      buff1New = true;
    }
    else if (buff2New == false)
    {
      for (int i = 0; i < BUFFSIZE; i++)
      {
        outBuffer2[i] = pi16Buffer[i];
      }
      buff2New = true;
    }
    else
    {
      //Used for debugging
      Serial.println("\n\rOver flow!");
    }

    // Start next conversion
    am_hal_pdm_dma_start(PDMHandle, &sTransfer);
  }
}

void setup()
{
  Serial.begin(500000);
  delay(10);
  initPDM(); //Setup and begin PDM interrupts
}

void loop()
{
  processFrame();
}

void processFrame()
{
  //Print any new data to serial port
  if (buff1New == true)
  {
    Serial.write((uint8_t *)outBuffer1, sizeof(outBuffer1));
    buff1New = false;
  }
  if (buff2New == true)
  {
    Serial.write((uint8_t *)outBuffer2, sizeof(outBuffer2));
    buff2New = false;
  }
}
