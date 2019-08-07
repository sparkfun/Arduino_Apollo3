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
    // Uart(uint8_t instance, ap3_gpio_pin_t pinRX, ap3_gpio_pin_t pinTX, ap3_gpio_pin_t pinRTS = AP3_UART_PIN_UNUSED, ap3_gpio_pin_t pinCTS = AP3_UART_PIN_UNUSED);
    bool begin(ap3_gpio_pin_t pinPDMData, ap3_gpio_pin_t pinPDMClock);
    bool available(void); //Goes true once an interrupt has occured

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

    void getData(uint32_t *PDMDataBuffer, uint32_t bufferSize);

    //void begin(void);
    // void begin(unsigned long baudrate, uint16_t config);
    // void begin(unsigned long baudrate, am_hal_uart_config_t config);
    // void end();
    // int available();
    // int availableForWrite();
    // int peek();
    // int read();
    // void flush();
    // size_t write(const uint8_t data);
    // size_t write(const uint8_t *buffer, size_t size);
    // uint32_t printf(const char *pcFmt, ...);
    // using Print::write; // pull in write(str) and write(buf, size) from Print

    void pdm_isr(void);

    // operator bool() { return true; } // todo: wait for a serial terminal to be open... probably depends on RTS or CTS...

private:
public: //temporary
    //AP3UartRingBuffer _rx_buffer; // These buffers guarantee the lifespan of the data to transmit
    //AP3UartRingBuffer _tx_buffer; //		to allow for asynchronous tranfsers

    //uint8_t _instance;
    void *_PDMhandle;
    am_hal_pdm_config_t _PDMconfig;
    ap3_gpio_pin_t _pinPDMData;
    ap3_gpio_pin_t _pinPDMClock;

    //ap3_err_t set_config(HardwareSerial_Config_e HWSconfig);
    ap3_err_t _begin(void); // call once all members + config structure are set up

    volatile bool _PDMdataReady = false;
};

#endif //_PDM_H_