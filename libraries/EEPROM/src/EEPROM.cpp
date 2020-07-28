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

#include "EEPROM.h"

#define DEFAULT_ADDRESS (0xFE000)
#define DEFAULT_SIZE (AM_HAL_FLASH_PAGE_SIZE)

// force EEPROM to use integer mutliples of pages aligned on page boundaries
#define EEPROM_ADDRESS(A) ((A/AM_HAL_FLASH_PAGE_SIZE) * AM_HAL_FLASH_PAGE_SIZE)
#define EEPROM_SIZE(S) (((S+(AM_HAL_FLASH_PAGE_SIZE-1))/AM_HAL_FLASH_PAGE_SIZE) * AM_HAL_FLASH_PAGE_SIZE)

EEPROM::EEPROM(uint32_t address, uint32_t size)
    FlashIAPBlockDevice(EEPROM_ADDRESS(address), EEPROM_SIZE(size))
{

}

EEPROM::EEPROM(void) :
    EEPROM(DEFAULT_ADDRESS, DEFAULT_SIZE)
{

}