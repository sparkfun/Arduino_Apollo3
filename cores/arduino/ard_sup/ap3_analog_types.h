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

#ifndef _AP3_ANALOG_TYPES_H_
#define _AP3_ANALOG_TYPES_H_

#define AP3_ANALOG_PADS 10
#define AP3_ANALOG_CHANNELS 15

enum EXTRA_ADC_PADS
{
    ADC_DIFF0 = 50, //AP3_VARIANT_NUM_PINS
    ADC_DIFF1,
    ADC_TEMP,
    ADC_DIV3,
    ADC_VSS,
};

typedef struct _ap3_analog_pad_map_elem_t
{
    ap3_gpio_pad_t pad;
    uint8_t funcsel;
} ap3_analog_pad_map_elem_t;

typedef struct _ap3_analog_channel_map_elem_t
{
    ap3_gpio_pad_t pad;
    am_hal_adc_slot_chan_e eChannel;
} ap3_analog_channel_map_elem_t;

typedef struct _ap3_analog_configure_map_elem_t
{
    ap3_gpio_pad_t pad;
    bool isAnalog;
} ap3_analog_configure_map_elem_t;

#endif // _AP3_ANALOG_TYPES_H_