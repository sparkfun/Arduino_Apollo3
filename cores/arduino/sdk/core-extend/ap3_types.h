/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// 'ap3_types.h' can be included in both C and C++ sources

#ifndef _ARDUINO_SDK_CORE_EXTEND_AP3_TYPES_H_
#define _ARDUINO_SDK_CORE_EXTEND_AP3_TYPES_H_

#include <stdint.h>

// Include Apollo headers
#include "am_mcu_apollo.h"
#include "am_util.h"
// #include "am_bsp.h"

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

// In v2.0.0 there is no more distinction between pins and pads. Now PinNames are an enum of prettified names. The number 
// associated with a pin is *always* the Apollo3 pad number
// typedef uint8_t ap3_gpio_pin_t;      // pins are the "prettified" names for connections on a dev board. 
// typedef uint8_t ap3_gpio_pad_t;      // pads refer directly to what's on the MCU
//                                      // Therefore you use the pinmap to find out the pad that is associated to the requested pin

#define digitalPinToInterrupt(P) (P)    // all apollo3 pads are interrupt capable

// Arduino compatibility
typedef uint8_t byte;
typedef bool boolean;

typedef void (*voidFuncPtr)(void);
typedef void (*voidFuncPtrArgs)(void*);

#endif // _ARDUINO_SDK_CORE_EXTEND_AP3_TYPES_H_