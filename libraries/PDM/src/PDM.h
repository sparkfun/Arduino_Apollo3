/*
Copyright (c) 2019 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef _PDM_H_
#define _PDM_H_

#include "Arduino.h"

#define AP3_GPIO_DEFAULT_PINCFG AP3_GPIO_PINCFG_NULL

//#define AP3_NUM_PDM 1
#define AP3_PDM_NUM_DATA_PADS 6
#define AP3_PDM_NUM_CLOCK_PADS 6

#ifndef MIC_DATA
#warning "Mic DATA pin not defined in variant. Using default."
#define MIC_DATA 36
#endif

#ifndef MIC_CLOCK
#warning "Mic CLOCK pin not defined in variant. Using default."
#define MIC_CLOCK 37
#endif

typedef struct _ap3_pdm_pad_map_elem_t
{
    ap3_gpio_pad_t pad;
    uint8_t funcsel;
} ap3_pdm_pad_map_elem_t;

typedef enum
{
    AP3_PDM_DATA = 0x00,
    AP3_PDM_CLOCK,
} ap3_pdm_pad_type_e;

const ap3_pdm_pad_map_elem_t ap3_pdm_data_map[AP3_PDM_NUM_DATA_PADS] = {
    {.pad = 11, .funcsel = AM_HAL_PIN_11_PDMDATA},
    {.pad = 15, .funcsel = AM_HAL_PIN_15_PDMDATA},
    {.pad = 29, .funcsel = AM_HAL_PIN_29_PDMDATA},
    {.pad = 34, .funcsel = AM_HAL_PIN_34_PDMDATA},
    {.pad = 36, .funcsel = AM_HAL_PIN_36_PDMDATA},
    {.pad = 45, .funcsel = AM_HAL_PIN_45_PDMDATA},
};

const ap3_pdm_pad_map_elem_t ap3_pdm_clock_map[AP3_PDM_NUM_CLOCK_PADS] = {
    {.pad = 10, .funcsel = AM_HAL_PIN_10_PDMCLK},
    {.pad = 12, .funcsel = AM_HAL_PIN_12_PDMCLK},
    {.pad = 14, .funcsel = AM_HAL_PIN_14_PDMCLK},
    {.pad = 22, .funcsel = AM_HAL_PIN_22_PDMCLK},
    {.pad = 37, .funcsel = AM_HAL_PIN_37_PDMCLK},
    {.pad = 46, .funcsel = AM_HAL_PIN_46_PDMCLK},
};

ap3_err_t ap3_pdm_pad_funcsel(ap3_pdm_pad_type_e type, ap3_gpio_pad_t pad, uint8_t *funcsel);

const am_hal_pdm_config_t ap3_pdm_config_default = {
    //Basic PDM setup pulled from SDK PDM example
    .eClkDivider = AM_HAL_PDM_MCLKDIV_1,
    .eLeftGain = AM_HAL_PDM_GAIN_0DB,
    .eRightGain = AM_HAL_PDM_GAIN_0DB,
    .ui32DecimationRate = 64,
    .bHighPassEnable = 0,
    .ui32HighPassCutoff = 0xB,
    .ePDMClkSpeed = AM_HAL_PDM_CLK_6MHZ,
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

class AP3_PDM
{
public:
    bool begin(ap3_gpio_pin_t pinPDMData = MIC_DATA, ap3_gpio_pin_t pinPDMClock = MIC_CLOCK);
    bool available(void); //Goes true if circular buffer is not empty
    bool isOverrun(void); //Goes true if head crosses tail

    bool setClockSpeed(am_hal_pdm_clkspd_e clockSpeed);
    am_hal_pdm_clkspd_e getClockSpeed();
    bool setClockDivider(am_hal_pdm_mclkdiv_e clockSpeed);
    am_hal_pdm_mclkdiv_e getClockDivider();
    bool setLeftGain(am_hal_pdm_gain_e gain);
    am_hal_pdm_gain_e getLeftGain();
    bool setRightGain(am_hal_pdm_gain_e gain);
    am_hal_pdm_gain_e getRightGain();
    bool setGain(am_hal_pdm_gain_e gain);
    bool setChannel(am_hal_pdm_chset_e channel);
    am_hal_pdm_chset_e getChannel();
    bool setDecimationRate(uint32_t deciRate);
    uint32_t getDecimationRate();

    bool updateConfig(am_hal_pdm_config_t newConfiguration);

    uint32_t getData(uint32_t *externalBuffer, uint32_t bufferSize);

    void pdm_isr(void);

private:
    void *_PDMhandle;
    am_hal_pdm_config_t _PDMconfig;
    ap3_gpio_pin_t _pinPDMData;
    ap3_gpio_pin_t _pinPDMClock;

    ap3_err_t _begin(void);

    //volatile bool _PDMdataReady = false;

    volatile bool _head = 0;
    volatile bool _tail = 0;
    volatile bool _overrun = false;

#define pdmDataBufferSize 512                            //Default is array of 4096 * 32bit
    volatile uint32_t _pdmDataBuffer[pdmDataBufferSize]; //This has been filled previous to ISR being called

#define circularBufferSize 4096
    volatile uint32_t _pdmCircularBuffer[circularBufferSize]; //This is filled by ISR and read by getData
};

#endif //_PDM_H_