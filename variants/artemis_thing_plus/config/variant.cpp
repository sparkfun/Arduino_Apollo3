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
    //Apollo Pad,      //Silkscreen indicator - Pin functions
    25,                //0     - ~RX1/SDA2/MISO2
    24,                //1     - ~TX1/32kHz/SWO
    44,                //2     - ~MOSI4
    35,                //3/A6  - ~A/TX1/I2SDAT/PDMCLK
    4,                 //4     - ~RX1/SLINT
    22,                //5     - ~PDMCLK/SWO
    23,                //6     - ~I2SWCLK/CMPOUT
    27,                //7     - ~SCL2/SCK2
    28,                //8     - ~MOSI2/I2SWCLK
    32,                //9/A7  - ~A/SCCIO
    14,                //10    -  SWCH/ADCD1N/TX1/PDMCLK
    7,                 //11    - ~MOSI0/CLKOUT
    6,                 //12    - ~MISO0/SDA0/I2SDAT
    5,                 //13    - ~SCK0/SCL0
    40,                //14    -  SDA4/MISO4/RX1
    39,                //15    - ~SCL4/SCK4/TX1
    43,                //16    - ~SDA3/MISO3/RX1
    42,                //17    - ~SCL3/SCK3/TX1
    26,                //18    - ~LED/SCCRST
    33,                //19/A0 - ~A/SWO/32KHZ
    13,                //20/A1 - ~A/I2SBCLK/RX1
    11,                //21/A2 - ~A/PDMDATA
    29,                //22/A3 - ~A/PDMDATA
    12,                //23/A4 - ~A/PDMCLK/TX1
	31,                //24/A5 - ~A/SCCCLK
	48,                //25    - Not exposed, TX0
	49,                //26    - Not exposed, RX0
    36,                //27    - Not exposed, PDMDATA of Mic
    37,                //28    - Not exposed, PDMCLK of Mic
};

// Uart Definitions
//Serial(instance, RX, TX)
Uart Serial(0, 26, 25); // Declares a Uart object called Serial using instance 0 of Apollo3 UART peripherals with RX on variant pin 26 and TX on pin 25 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)
Uart Serial1(1, 0, 1);  // Declares a Uart object called Serial1 using instance 1 of Apollo3 UART peripherals with RX on pin 0 and TX on pin 1 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)

// Analog pin numbers
const ap3_gpio_pin_t ap3_analog_A0 = 19;
const ap3_gpio_pin_t ap3_analog_A1 = 20;
const ap3_gpio_pin_t ap3_analog_A2 = 21;
const ap3_gpio_pin_t ap3_analog_A3 = 22;
const ap3_gpio_pin_t ap3_analog_A4 = 23;
const ap3_gpio_pin_t ap3_analog_A5 = 24;
const ap3_gpio_pin_t ap3_analog_A6 = 3;
const ap3_gpio_pin_t ap3_analog_A7 = 9;
