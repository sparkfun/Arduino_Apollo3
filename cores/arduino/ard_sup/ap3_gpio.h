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
#ifndef _AP3_GPIO_H_
#define _AP3_GPIO_H_

#include "Arduino.h"

// User Config
#define AP3_GPIO_MAX_EXT_INT_HANDLERS AP3_GPIO_MAX_PADS
// End User Config

//GPIO FUNCTIONS
extern const am_hal_gpio_pincfg_t AP3_GPIO_PINCFG_NULL;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_WITH_READ_12;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OPEN_DRAIN_WITH_READ_12;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLDOWN;

// macros pointing to internal apollo3 GPIO pincfg structures
#define AP3_PINCFG_INPUT            (g_AM_HAL_GPIO_INPUT)
#define AP3_PINCFG_OUTPUT           (g_AM_HAL_GPIO_OUTPUT_WITH_READ_12)
#define AP3_PINCFG_INPUT_PULLUP     (g_AM_HAL_GPIO_INPUT_PULLUP)
#define AP3_PINCFG_INPUT_PULLDOWN   (g_AM_HAL_GPIO_INPUT_PULLDOWN)
#define AP3_PINCFG_OPEN_DRAIN       (g_AM_HAL_GPIO_OPEN_DRAIN_WITH_READ_12)
#define AP3_PINCFG_TRISTATE         (g_AM_HAL_GPIO_TRISTATE)

// constants for Arduino pin modes
#define INPUT           (0x00)
#define OUTPUT          (0x01)
#define INPUT_PULLUP    (0x02)
#define INPUT_PULLDOWN  (0x03)
#define OPEN_DRAIN      (0x04)
#define TRISTATE        (0x05)

#define AP3_GPIO_MAX_PADS (50)
#define AP3_GPIO_IS_VALID(pad) ((pad >= 0) && (pad < AP3_GPIO_MAX_PADS))

extern ap3_gpio_pad_t ap3_gpio_pin2pad(ap3_gpio_pin_t pin);
#define AP3_GPIO_PAD_UNUSED (255)

#define AP3_GPIO_DEFAULT_PINCFG AP3_GPIO_PINCFG_NULL

// apollo3 pad capabilities
inline bool ap3_gpio_is_valid(ap3_gpio_pad_t pad) { return AP3_GPIO_IS_VALID(pad); }
inline bool ap3_gpio_has_gpio(ap3_gpio_pad_t pad) { return AP3_GPIO_IS_VALID(pad); }

// Interrupt enable/disable function
uint32_t ap3_gpio_enable_interrupts(uint32_t ui32Pin, uint32_t eIntDir);

void padMode(uint8_t pad, am_hal_gpio_pincfg_t mode);
void padMode(uint8_t pad, am_hal_gpio_pincfg_t mode, ap3_err_t *retval);

void pinMode(uint8_t pin, uint8_t mode);
void pinMode(uint8_t pin, am_hal_gpio_pincfg_t mode);
void pinMode(uint8_t pin, am_hal_gpio_pincfg_t mode, ap3_err_t *retval);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

void attachInterrupt(uint8_t pin, voidFuncPtr callback, int mode);
void attachInterruptArg(uint8_t pin, voidFuncPtrArgs callbackArgs, void *arg, int mode);
void detachInterrupt(uint8_t pin);

unsigned long pulseIn(uint8_t pinNumber, uint8_t state, unsigned long timeout = 1000000L);
unsigned long pulseInLong(uint8_t pinNumber, uint8_t state, unsigned long timeout);

#endif //_AP3_GPIO_H_