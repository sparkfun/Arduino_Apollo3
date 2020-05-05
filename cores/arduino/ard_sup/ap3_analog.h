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

#ifndef _AP3_ANALOG_H_
#define _AP3_ANALOG_H_

#include "Arduino.h"
#include "ap3_analog_types.h"

#define AP3_USE_DEFAULT_TIMER_SEG (0xFF)
#define AP3_USE_DEFAULT_TIMER_NUM (0xFF)

extern const ap3_analog_pad_map_elem_t ap3_analog_map[AP3_ANALOG_PADS];
extern const ap3_analog_channel_map_elem_t ap3_analog_channel_map[AP3_ANALOG_CHANNELS];
extern ap3_analog_configure_map_elem_t ap3_analog_configure_map[AP3_ANALOG_CHANNELS];

// ADC Device Handle.
static void *g_ADCHandle;

ap3_err_t ap3_adc_setup(void);
ap3_err_t ap3_set_pin_to_analog(uint8_t pinNumber);
ap3_err_t ap3_analog_pad_funcsel(ap3_gpio_pad_t padNumber, uint8_t *funcsel);
ap3_err_t ap3_change_channel(ap3_gpio_pad_t padNumber);

bool power_adc_disable();
uint16_t analogRead(uint8_t pinNumber);
ap3_err_t analogReadResolution(uint8_t bits);
float getInternalTemp();

ap3_err_t ap3_pwm_output(uint8_t pin, uint32_t th, uint32_t fw, uint32_t clk);
ap3_err_t analogWriteResolution(uint8_t res);
ap3_err_t analogWriteFrameWidth(uint32_t fw);
ap3_err_t analogWriteFrequency(float freq);
ap3_err_t analogWrite(uint8_t pin, uint32_t val);
ap3_err_t servoWriteResolution(uint8_t res);
uint8_t getServoResolution();
ap3_err_t servoWrite(uint8_t pin, uint32_t val);
ap3_err_t servoWrite(uint8_t pin, uint32_t val, uint16_t minMicros, uint16_t maxMicros);

ap3_err_t tone(uint8_t pin, uint32_t freq);
ap3_err_t tone(uint8_t pin, uint32_t freq, uint32_t duration);
ap3_err_t noTone(uint8_t pin);

#endif // _AP3_ANALOG_H_