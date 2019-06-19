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

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif // abs

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define abs(x) ((x) > 0 ? (x) : -(x))
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define round(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x) * (x))

// constants
#define LOW (0x0)
#define HIGH (0x1)

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

typedef bool boolean;

enum BitOrder
{
    LSBFIRST = 0,
    MSBFIRST = 1
};

#include "ap3_types.h"

// some libraries and sketches depend on this AVR stuff,
// assuming Arduino.h or WProgram.h automatically includes it...
//
#include "avr/pgmspace.h"
#include "avr/interrupt.h"
#include "avr/io.h"

#include "ap3_timing.h"
#include "ap3_gpio.h"
#include "ap3_debugging.h"
#include "ap3_uart.h"
#include "ap3_analog.h"
#include "WMath.h"
#include "WCharacter.h"

#include "variant.h"

#endif // _ARDUINO_H_
