/*
  ap3_shift taken from wiring_shift.c by David Mellis.
  Original copyright notice below.

  wiring_shift.c - shiftOut() function
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#include "ap3_shift.h"

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder)
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; ++i)
    {
        digitalWrite(clockPin, HIGH);
        if (bitOrder == LSBFIRST)
            value |= digitalRead(dataPin) << i;
        else
            value |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, LOW);
    }
    return value;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        if (bitOrder == LSBFIRST)
            digitalWrite(dataPin, !!(val & (1 << i)));
        else
            digitalWrite(dataPin, !!(val & (1 << (7 - i))));

        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}

// Configure a pin to be used in fast mode
void enableFastShift(uint8_t dataPin, uint8_t clockPin)
{
    uint8_t dataPadNumber = ap3_gpio_pin2pad(dataPin);
    uint8_t clockPadNumber = ap3_gpio_pin2pad(clockPin);

    am_hal_gpio_fastgpio_disable(dataPadNumber);
    am_hal_gpio_fastgpio_clr(dataPadNumber);

    am_hal_gpio_fastgpio_disable(clockPadNumber);
    am_hal_gpio_fastgpio_clr(clockPadNumber);

    am_hal_gpio_fast_pinconfig((uint64_t)0x1 << dataPadNumber |
                                   (uint64_t)0x1 << clockPadNumber,
                               g_AM_HAL_GPIO_OUTPUT_12, 0);
}

void fastShiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
    uint8_t dataPadNumber = ap3_gpio_pin2pad(dataPin);
    uint8_t clockPadNumber = ap3_gpio_pin2pad(clockPin);

    for (uint8_t i = 0; i < 8; i++)
    {
        if (bitOrder == LSBFIRST)
        {
            if (val & (1 << i))
                am_hal_gpio_fastgpio_set(dataPadNumber);
            else
                am_hal_gpio_fastgpio_clr(dataPadNumber);
        }
        else
        {
            if (val & (1 << (7 - i)))
                am_hal_gpio_fastgpio_set(dataPadNumber);
            else
                am_hal_gpio_fastgpio_clr(dataPadNumber);
        }

        am_hal_gpio_fastgpio_set(clockPadNumber);
        am_hal_gpio_fastgpio_clr(clockPadNumber);
    }
}

uint8_t fastShiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder)
{
    uint8_t dataPadNumber = ap3_gpio_pin2pad(dataPin);
    uint8_t clockPadNumber = ap3_gpio_pin2pad(clockPin);

    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; ++i)
    {
        am_hal_gpio_fastgpio_set(clockPadNumber);
        if (bitOrder == LSBFIRST)
            value |= am_hal_gpio_fastgpio_read(dataPadNumber) << i;
        else
            value |= am_hal_gpio_fastgpio_read(dataPadNumber) << (7 - i);
        am_hal_gpio_fastgpio_clr(clockPadNumber);
    }
    return value;
}