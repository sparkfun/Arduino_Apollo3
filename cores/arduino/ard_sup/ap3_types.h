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

// 'ap3_types.h' can be included in both C and C++ sources

#ifndef _AP3_TYPES_H_
#define _AP3_TYPES_H_

#include <stdint.h>

// Include Apollo headers
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "am_bsp.h"

typedef enum {
    AP3_OK = 0x00,          // One singular "OK" value.
    AP3_ERR,                // All others represent particular errors
    AP3_INVALID_HANDLE,
    AP3_IN_USE,
    AP3_TIMEOUT,
    AP3_OUT_OF_RANGE,
    AP3_INVALID_ARG,
    AP3_INVALID_OPERATION,
    AP3_MEM_ERR,
    AP3_HW_ERR,             // Up to AP3_HW_ERR map directly to the values in 'am_hal_status_e'
    // discontinuity
    AP3_MODULE_SPECIFIC_START = 0x08000000,
}ap3_err_t;

typedef uint8_t ap3_gpio_pin_t;     // pins are the "prettified" names for connections on a dev board. 
typedef uint8_t ap3_gpio_pad_t;     // pads refer directly to what's on the MCU
                                    // Therefore you use the pinmap to find out the pad that is associated to the requested pin


typedef uint8_t byte;

#endif // _AP3_TYPES_H_