/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
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

float getTempDegC( void );
float getTempDegF( void );
float getVCCV( void );

uint32_t initializeADC( void );

#endif // _ARDUINO_SDK_CORE_EXTEND_ANALOG_INTERNAL_H_
