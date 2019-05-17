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


// If not noted, pads are "unused" simply because they are not broken out from the Artemis module to a pin on the BlackBoard
const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS] = {
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    16,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    20,                     // SWDCK
    21,                     // SWDIO
    22,
    23,
    24,
    25,
    AP3_GPIO_PAD_UNUSED,
    27,
    28,
    29,
    AP3_GPIO_PAD_UNUSED,    // pad 30 is not exposed on the Artemis module        
    31,                     
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    AP3_GPIO_PAD_UNUSED,
    42,
    43,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,
    AP3_GPIO_PAD_UNUSED,    // pad 46 is not exposed on the Artemis module
    AP3_GPIO_PAD_UNUSED,    // pad 47 is used for the boot signal and is not exposed to the user on the BlackBoard (but can be exposed on other Artemis designs)
    48,                     // U0TX
    49,                     // U0RX
};

// Uart Definitions
Uart Serial(0, 49, 48);// declares a Uart object called Serial using instance 0 of Apollo3 UART peripherals with RX on pin 49 and TX on pin 48 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)
