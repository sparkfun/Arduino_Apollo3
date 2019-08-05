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

// The "Arduino.h" header file is intended to only be included by C++ sources.

#ifndef _ARDUINO_H_
#define _ARDUINO_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#ifdef OVERFLOW
#define ARDUINO_REDEFINE_OVERFLOW OVERFLOW
#undef OVERFLOW
#warning "OVERFLOW is defined already... trying to save it, include Apollo3 headers, then restore it"
#endif

// Include Apollo headers
#include "am_mcu_apollo.h"
#include "am_util.h"
    // #include "am_bsp.h"

#include <math.h> //Gets us pow()

#ifdef ARDUINO_REDEFINE_OVERFLOW
#warning "restoring OVERFLOW from value in ARDUINO_REDEFINE_OVERFLOW"
#define OVERFLOW ARDUINO_REDEFINE_OVERFLOW
#undef ARDUINO_REDEFINE_OVERFLOW
#endif

    /* system functions */
    int main(void);
    // void init(void);

    /* sketch */
    void setup(void);
    void loop(void);

#ifdef __cplusplus
} // extern "C"
#endif

#include "Arduino_defines.h"
#include "ap3_types.h"

// some libraries and sketches depend on this AVR stuff,
// assuming Arduino.h or WProgram.h automatically includes it...
#include "avr/pgmspace.h"
#include "avr/interrupt.h"
#include "avr/io.h"

#include "ap3_interrupts.h"
#include "ap3_timing.h"
#include "ap3_gpio.h"
#include "ap3_debugging.h"
#include "ap3_uart.h"
#include "ap3_analog.h"
#include "ap3_clock_sources.h"
#include "ap3_shift.h"
#include "ap3_pdm.h"
#include "WMath.h"
#include "WCharacter.h"

#include "variant.h"

#include "ap3_post_variant.h"

#endif // _ARDUINO_H_
