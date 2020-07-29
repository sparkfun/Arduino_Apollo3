/*
Copyright (c) 2020 SparkFun Electronics

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

#ifndef _ARDUINO_SDK_CORE_EXTEND_ANALOG_INTERNAL_H_
#define _ARDUINO_SDK_CORE_EXTEND_ANALOG_INTERNAL_H_

#include "Arduino.h"

typedef enum _ap3_adc_internal_channel_e {
    AP3_ADC_INTERNAL_CHANNELS_DIFF0 = 0x00,
    AP3_ADC_INTERNAL_CHANNELS_DIFF1,
    AP3_ADC_INTERNAL_CHANNELS_TEMP,
    AP3_ADC_INTERNAL_CHANNELS_VCC_DIV3,
    AP3_ADC_INTERNAL_CHANNELS_VSS,

    // number of internal channels
    AP3_ADC_INTERNAL_CHANNELS_NUM
} ap3_adc_internal_channel_e;

typedef struct _ap3_adc_channel_config_t {
    ap3_gpio_pad_t pinNumber;
    ap3_adc_internal_channel_e internal_channel;
    am_hal_adc_slot_chan_e eChannel;
    uint8_t funcsel;
} ap3_adc_channel_config_t;

extern ap3_adc_channel_config_t ap3_adc_channel_configs[];

int analogReadDiff( uint32_t instance );
int analogReadTemp( void );
int analogReadVCCDiv3( void );
int analogReadVSS( void );

uint32_t initializeADC( void );

#endif // _ARDUINO_SDK_CORE_EXTEND_ANALOG_INTERNAL_H_
