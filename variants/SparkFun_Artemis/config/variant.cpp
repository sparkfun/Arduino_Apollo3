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

// This is the actual pinmap.
// For a variant pin X the corresponding Apollo3 pad Y is stored in ap3_variant_pinmap[X]
// X [0, (AP3_VARIANT_NUM_PINS - 1)]
// Y [0, 49]
// If you have an unused *pin* number use AP3_GPIO_PAD_UNUSED as the *pad* number
//
// Because the Artemis Module variant is meant to replicate the Apollo3 microcontroller
// this pinmap will be a direct mapping. An example of an indirect mapping is provided at the bottom of this file (commented out)
const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS] = {
    //On the Artemis Mega, what you see is what you get pin-to-pad on the Apollo3
    0,
    1,
    2,
    3,
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
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    AP3_GPIO_PAD_UNUSED, //The one pad not broken out of the Apollo3 on the Artemis
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
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
};

// Uart Definitions
//Serial(instance, RX, TX)
//We default Serial to 22/23 but can be moved to any TX0/RX0 pin
Uart Serial(0, 23, 22); // Declares a Uart object called Serial using instance 0 of Apollo3 UART peripherals with RX on variant pin 23 and TX on pin 24 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)
                        // Uart Serial1(1, 0, 1);  // Declares a Uart object called Serial1 using instance 1 of Apollo3 UART peripherals with RX on pin 0 and TX on pin 1 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)

// Pin aliasing using the const uint8_t approach
//
// const uint8_t A0 = 16;   // means that "A0" is equivalent to saying *pin* 16 (which culd map to any Apollo3 *pad*)
// const uint8_t A1 = 17;
// const uint8_t A2 = 18;
// const uint8_t A3 = 19;
// const uint8_t A4 = 20;
// const uint8_t A5 = 21;

// Indirect pinmap example
//
// const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS] = {
//     // AP3_GPIO_PAD_UNUSED  // use when the user should not try to use a given *pin* number

//     //~ = PWM, A = ADC
//     //Apollo Pad,      //Silkscreen indicator - Pin functions
//     25,                     //0 - ~RX1/SDA2/MISO2
//     24,                     //1 - ~TX1/32kHz/SWO
//     35,                     //2/A6 - ~A/TX1/I2SDAT/PDMCLK
//     4,                      //3 - ~RX1/SLINT
//     22,                     //4 - ~PDMCLK/SWO
//     23,                     //5 - ~I2SWCLK/CMPOUT
//     27,                     //6 - ~SCL2/SCK2
//     28,                     //7 - ~MOSI2/I2SWCLK
//     32,                     //8/A8 - ~A/SCCIO
//     12,                     //9/A9 - ~A/PDMCLK/TX1
//     13,                     //10/A10 - ~A/I2SBCLK/RX1
//     7,                      //11 - ~MOSI0/CLKOUT
//     6,                      //12 - ~MISO0/SDA0/I2SDAT
//     5,                      //13 - ~LED/SCK0/SCL0
//     40,                     //14 - SDA4/MISO4/RX1
//     39,                     //15 - ~SCL4/SCK4/TX1
//     29,                     //16/A0 - ~A/PDMDATA
//     11,                     //17/A1 - ~A/PDMDATA
//     34,                     //18/A2 - A/CMPRF2/PDMDATA
//     33,                     //19/A3 - ~A/SWO/32kHz
//     16,                     //20/A4 - A/TRIG0/SCCRST
//     31,                     //21/A5 - ~A/SCCCLK
//     48,                     //22 - ~TX0/SCL5/SCK5
//     49,                     //23 - ~RX0/SDA5/MISO5
//     AP3_GPIO_PAD_UNUSED,    //24 - Demonstrates what to do if the user should not try to access a *pin* called 24
//     9,                      //25 - Solder pad, SDA1/MISO1/RX1/SCCIO
//     10,                     //26 - Solder pad, MOSI1/TX1/PDMCLK
//     38,                     //27 - Solder pad, MOSI3/TX1
//     42,                     //28 - Solder pad, ~SCL3/SCK3/TX1
//     43,                     //29 - Solder pad, ~SDA3/MISO3/RX1
//     36,                     //30 - Not exposed, PDMDATA of Mic
//     37,                     //31 - Not exposed, PDMCLK of Mic
//     AP3_ADC_DIFF0_PAD,      //32 - Not a real pad, ADC_DIFF0
//     AP3_ADC_DIFF1_PAD,      //33 - Not a real pad, ADC_DIFF1
//     AP3_ADC_TEMP_PAD,       //34 - Not a real pad, ADC_TEMP
//     AP3_ADC_DIV3_PAD,       //35 - Not a real pad, ADC_DIV3
//     AP3_ADC_VSS_PAD,        //36 - Not a real pad, ADC_VSS
// };