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

#include "ap3_types.h"
//#include "ap3_analog.h"
#include "ap3_analog_types.h"

const ap3_analog_pad_map_elem_t ap3_analog_map[AP3_ANALOG_PADS] = {
    {.pad = 11, .funcsel = AM_HAL_PIN_11_ADCSE2},
    {.pad = 12, .funcsel = AM_HAL_PIN_12_ADCD0NSE9},
    {.pad = 13, .funcsel = AM_HAL_PIN_13_ADCD0PSE8},
    {.pad = 16, .funcsel = AM_HAL_PIN_16_ADCSE0},
    {.pad = 29, .funcsel = AM_HAL_PIN_29_ADCSE1},
    {.pad = 31, .funcsel = AM_HAL_PIN_31_ADCSE3},
    {.pad = 32, .funcsel = AM_HAL_PIN_32_ADCSE4},
    {.pad = 33, .funcsel = AM_HAL_PIN_33_ADCSE5},
    {.pad = 34, .funcsel = AM_HAL_PIN_34_ADCSE6},
    {.pad = 35, .funcsel = AM_HAL_PIN_35_ADCSE7},
};

//By default, all pins are GPIO at POR
//This keeps track of what pads have function set to analog input
ap3_analog_configure_map_elem_t ap3_analog_configure_map[AP3_ANALOG_CHANNELS] = {
    {.pad = 11, .isAnalog = false},
    {.pad = 12, .isAnalog = false},
    {.pad = 13, .isAnalog = false},
    {.pad = 16, .isAnalog = false},
    {.pad = 29, .isAnalog = false},
    {.pad = 31, .isAnalog = false},
    {.pad = 32, .isAnalog = false},
    {.pad = 33, .isAnalog = false},
    {.pad = 34, .isAnalog = false},
    {.pad = 35, .isAnalog = false},
    {.pad = AP3_ADC_DIFF0_PAD, .isAnalog = false},
    {.pad = AP3_ADC_DIFF1_PAD, .isAnalog = false},
    {.pad = AP3_ADC_TEMP_PAD, .isAnalog = true},
    {.pad = AP3_ADC_DIV3_PAD, .isAnalog = true},
    {.pad = AP3_ADC_VSS_PAD, .isAnalog = true},
};

const ap3_analog_channel_map_elem_t ap3_analog_channel_map[AP3_ANALOG_CHANNELS] = {
    {.pad = 11, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE2},
    {.pad = 12, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE9},
    {.pad = 13, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE8},
    {.pad = 16, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE0},
    {.pad = 29, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE1},
    {.pad = 31, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE3},
    {.pad = 32, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE4},
    {.pad = 33, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE5},
    {.pad = 34, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE6},
    {.pad = 35, .eChannel = AM_HAL_ADC_SLOT_CHSEL_SE7},
    {.pad = AP3_ADC_DIFF0_PAD, .eChannel = AM_HAL_ADC_SLOT_CHSEL_DF0},
    {.pad = AP3_ADC_DIFF1_PAD, .eChannel = AM_HAL_ADC_SLOT_CHSEL_DF1},
    {.pad = AP3_ADC_TEMP_PAD, .eChannel = AM_HAL_ADC_SLOT_CHSEL_TEMP},
    {.pad = AP3_ADC_DIV3_PAD, .eChannel = AM_HAL_ADC_SLOT_CHSEL_BATT},
    {.pad = AP3_ADC_VSS_PAD, .eChannel = AM_HAL_ADC_SLOT_CHSEL_VSS},
};
