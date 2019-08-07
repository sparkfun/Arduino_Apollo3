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

#include "variant.h"

// Apollo3 pads 30 and 46 are not exposed by Artemis module
const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS] = {
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    5, //Camera D5
    6, //Camera D4
    7, //Camera PCLK
    8, //Camera SCL
    9, //Camera SDA
    AP3_GPIO_PAD_UNUSED,
    11, //Camera D1
    12, //Mic CLK
    13, //Camera Trig
    14, //Camera D0
    15, //Camera VSync
    16, //GPIO 16
    17, //LED Green
    18, //LED Blue
    19, //LED Red
    20, //SWDCK
    21, //SWDIO
    AP3_GPIO_PAD_UNUSED,
    23, //Camera INT
    AP3_GPIO_PAD_UNUSED,
    25, //Camera D2
    26, //Camera MCLK
    27, //Camera HSync
    28, //Camera D7
    29, //Mic DATA
    AP3_GPIO_PAD_UNUSED,
    31, //GPIO 31
    32, //Camera 1.8V Enable
    33, //GPIO 33
    34, //Camera D3
    35, //Camera D6
    AP3_GPIO_PAD_UNUSED,
    37, //LED Yellow
    AP3_GPIO_PAD_UNUSED,
    39, //Qwiic SCL
    40, //Qwiic SDA
    AP3_GPIO_PAD_UNUSED,
    42, //Accel SCL
    43, //Accel SDA
    44, //GPIO 44
    45, //GPIO 45
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    48, //Boot TX
    49, //Boot RX
};

// Uart Definitions
Uart Serial(0, 49, 48); // declares a Uart object called Serial using instance 0 of Apollo3 UART peripherals with RX on pin 49 and TX on pin 48 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)
