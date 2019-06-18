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
#ifndef _AP3_VARIANT_H_
#define _AP3_VARIANT_H_

#include "Arduino.h"

#define AP3_VARIANT_NUM_PINS (50)

// Pin map declaration
extern const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS];

// Uart declarations
class Uart;         // Forward declaration of Uart
extern Uart Serial;


Todo: determine IOMaster allocation for Edge V2
// Wire defines
#define WIRE_INTERFACES_COUNT 3

#define WireQwiic Wire  // Giving Wire an alias of "WireQwiic" in case people want to use it
#define AP3_Wire_IOM 4  // Secify that Wire uses IOMaster instance 4

#define WireAccel Wire1
#define AP3_Wire1_IOM 3

#define WireCamera Wire2
#define AP3_Wire1_IOM 2

// SPI Defines
#define SPI_INTERFACES_COUNT 0          //Edge V2 doesn't have any fully-exposed SPI ports


#define LED_BUILTIN 19

#endif // _AP3_VARIANT_H_