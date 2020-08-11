/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
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
