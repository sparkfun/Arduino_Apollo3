/*
Copyright (c) 2020 SparkFun Electronics

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

#define PinMode Arduino_PinMode
#include "core-api/api/Common.h"
#undef PinMode

#include "core-extend/Common.h"

#define standInFunc() printf("Stand-In for '%s' [file: %s, line: %d]\n", __FUNCTION__, __FILE__, __LINE__)

#define MAX_PULSE_IN_TIMEOUT_US (3 * 60 * 1000 * 1000)

unsigned long indexPulseIn(pin_size_t index, uint8_t state, unsigned long timeout){
    pin_size_t pinNumber = pinNumberByIndex(index);
    if(pinNumber == (pin_size_t)NC){ return 0; }

    if(timeout > MAX_PULSE_IN_TIMEOUT_US){
        timeout = MAX_PULSE_IN_TIMEOUT_US;
    }

    // Enable FastGPIO
    am_hal_gpio_fastgpio_disable(pinNumber);
    am_hal_gpio_fastgpio_clr(pinNumber);
    am_hal_gpio_fast_pinconfig((uint64_t)0x1 << pinNumber, g_AM_HAL_GPIO_OUTPUT_WITH_READ, 0);

    uint32_t t_start = micros();
    uint32_t t_stop = 0;
    while (am_hal_gpio_fastgpio_read(pinNumber) == state){ //Wait for previous pulse to end{
        if (micros() > (t_start + timeout)){
            return (0); //Pulse did not end
        }
    }
    while (am_hal_gpio_fastgpio_read(pinNumber) != state){ //Wait for pin to change state
        if (micros() > (t_start + timeout)){
            return (0); //Pulse did not start
        }
    }
    t_start = micros();
    while (am_hal_gpio_fastgpio_read(pinNumber) == state){ //Wait for pin to exit state
        if (micros() > (t_start + timeout)){
            return (0); //Pulse did not end
        }
    }
    t_stop = micros();

    // Disable FastGPIO
    am_hal_gpio_fastgpio_disable(pinNumber);

    return (t_stop - t_start);
}
