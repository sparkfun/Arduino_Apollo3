/*
  This is a library written for the SparkFun Artemis

  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!
  https://www.sparkfun.com/products/15332
  https://www.sparkfun.com/products/15376
  https://www.sparkfun.com/products/15411
  https://www.sparkfun.com/products/15412

  Written by Nathan Seidle @ SparkFun Electronics, June 16th, 2019

  Pseudo-EEPROM on the Cortex-M4F

  https://github.com/sparkfun/SparkFun_Apollo3

  There is no EEPROM built into the Apollo3 Cortex-M4F but we have 1M of flash
  so we carve out the last 8k of flash for EEPROM space. Pages are erased in
  8k chunks so 8k of EEPROM works.

  Flash is 0x00 to 0xFF000. EEPROM writes will start at 0xFF000 - 8192 = 0xF2000.

  Page erase takes 15ms
  Writing a byte takes 19ms
  Writing a float across two words takes 19ms
  Update (no write) takes ~1ms

  Development environment specifics:
  Arduino IDE 1.8.x

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "EEPROM.h"
#include "Arduino.h"

//Write a byte to a given "EEPROM" location
void write(uint16_t eepromLocation, uint8_t dataToWrite)
{
  writeBlockToEEPROM(eepromLocation, &dataToWrite, 1);
}

//Read a byte from a given location in "EEPROM"
uint8_t read(uint16_t eepromLocation)
{
  uint32_t flashLocation = AP3_FLASH_EEPROM_START + eepromLocation;
  return (*(uint8_t *)flashLocation);
}

//Erase 8k page encapsulating the EEPROM section
void EEPROMClass::erase()
{
  am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                          AM_HAL_FLASH_ADDR2INST(AP3_FLASH_EEPROM_START),
                          AM_HAL_FLASH_ADDR2PAGE(AP3_FLASH_EEPROM_START));
}

//1) Make copy of current flash contents into SRAM
//2) Record user data into SRAM.
//3) Check if new data is different from flash.
//4) Erase flash page (8k)
//5) Write SRAM back into flash
void writeBlockToEEPROM(uint16_t eepromLocation, const uint8_t *dataToWrite, uint16_t blockSize)
{
  //Error check
  if (eepromLocation + blockSize >= AP3_FLASH_EEPROM_SIZE)
  {
    blockSize = AP3_FLASH_EEPROM_SIZE - eepromLocation;
  }

  //Read the contents of current "EEPROM" to SRAM
  //Flash is written in 32-bit words but user passes in array of bytes
  //Create an array of 32-bit words but reference it a byte at a time
  uint32_t flashContent[AP3_FLASH_EEPROM_SIZE / 4];

  //We can't read 32bits at a time because the way flash is oriented (little endian)
  //So we read a byte at a time
  uint8_t *eepromContents = (uint8_t *)flashContent;
  for (uint16_t x = 0; x < AP3_FLASH_EEPROM_SIZE; x++)
  {
    eepromContents[x] = *(uint8_t *)(AP3_FLASH_EEPROM_START + x);
  }

  //Write the caller's new data into the byte array
  for (uint16_t x = 0; x < blockSize; x++)
  {
    eepromContents[eepromLocation + x] = dataToWrite[x];
  }

  //Run a check here to see if the new data is the same as what's in flash. If it's the same,
  //just return, don't erase flash.
  bool theSame = true;
  for (uint16_t x = 0; x < AP3_FLASH_EEPROM_SIZE; x++)
  {
    if (eepromContents[x] != *(uint8_t *)(AP3_FLASH_EEPROM_START + x))
    {
      theSame = false;
      break;
    }
  }
  if (theSame == true)
    return;

  //Then we erase an 8K page
  am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                          AM_HAL_FLASH_ADDR2INST(AP3_FLASH_EEPROM_START + eepromLocation),
                          AM_HAL_FLASH_ADDR2PAGE(AP3_FLASH_EEPROM_START + eepromLocation));

  //Then we write the contents of the array back
  am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                            flashContent,
                            (uint32_t *)AP3_FLASH_EEPROM_START,
                            AP3_FLASH_EEPROM_SIZE / 4);
}
