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

const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS] = {
    //~ = PWM, A = ADC
    //Apollo Pad, //Silkscreen indicator
    13, //A0 - ~I2SBCLK/RX1
    33, //A1 - ~SWO/32kHz
    11, //A2 - ~PDMDATA
    29, //A3 - ~PDMDATA
    18, //4 - ~CMPIN1/SCCIO
    31, //A5 - ~SCCCLK
    43, //6 - ~SDA3/MISO3/RX1
    42, //7 - SCL3/SCK3/TX1
    38, //8 - MOSI3/RX1
    39, //9 - ~SCL4/SCK4/TX1
    40, //10 - SDA4/MISO4/RX1
    5,  //11 - ~SCL0/SCK0
    7,  //12 - ~MOSI0/CLKOUT
    6,  //13 - ~SDA0/MISO0/I2SDAT
    35, //A14 - ~TX1/I2SDAT/PDMCLK
    32, //A15 - ~SCCIO
    12, //A16 - ~PDMCLK/TX1
    25, //17 - ~SDA2/MISO2/RX1
    27, //18 - ~SCL2/SCK2
    19, //19 - ~Not exposed, Status LED
    48, //20 - Not exposed, TX0
    49, //21 - Not exposed, RX0
    36, //22 - Not exposed, PDMDATA of Mic
    37, //23 - Not exposed, PDMCLK of Mic
};

// Uart Definitions
//Serial(instance, RX, TX)
Uart Serial(0, 21, 20); // Declares a Uart object called Serial using instance 0 of Apollo3 UART peripherals with RX on variant pin 49 and TX on pin 48 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)
Uart Serial1(1, 10, 9); // Declares a Uart object called Serial1 using instance 1 of Apollo3 UART peripherals with RX on pin 10 and TX on pin 9 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)

// Analog pin numbers
const ap3_gpio_pin_t ap3_analog_A0 = 0;
const ap3_gpio_pin_t ap3_analog_A1 = 1;
const ap3_gpio_pin_t ap3_analog_A2 = 2;
const ap3_gpio_pin_t ap3_analog_A3 = 3;
const ap3_gpio_pin_t ap3_analog_A5 = 5;
const ap3_gpio_pin_t ap3_analog_A14 = 14;
const ap3_gpio_pin_t ap3_analog_A15 = 15;
const ap3_gpio_pin_t ap3_analog_A16 = 16;
