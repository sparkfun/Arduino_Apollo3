/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "EEPROM.h"

EEPROMClass EEPROM;

#define DEFAULT_ADDRESS (0xFE000)
#define DEFAULT_SIZE (AM_HAL_FLASH_PAGE_SIZE)

// force EEPROM to use integer mutliples of pages aligned on page boundaries
#define EEPROM_ADDRESS(A) ((A/AM_HAL_FLASH_PAGE_SIZE) * AM_HAL_FLASH_PAGE_SIZE)
#define EEPROM_SIZE(S) (((S+(AM_HAL_FLASH_PAGE_SIZE-1))/AM_HAL_FLASH_PAGE_SIZE) * AM_HAL_FLASH_PAGE_SIZE)

EEPROMClass::EEPROMClass(uint32_t address, uint32_t size) :
    FlashIAPBlockDevice(EEPROM_ADDRESS(address), EEPROM_SIZE(size))
{

}

EEPROMClass::EEPROMClass(void) :
    FlashIAPBlockDevice(DEFAULT_ADDRESS, DEFAULT_SIZE)
{

}