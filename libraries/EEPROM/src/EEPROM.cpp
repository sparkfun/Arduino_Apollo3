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
  Writing a byte takes 30ms
  Writing a float across two words takes 30ms
  Update (no write) takes 1ms

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

//Constructor
ap3_EEPROM::ap3_EEPROM()
{
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//Write a byte to a given "EEPROM" location
//Automatically masks user's byte into flash without
//affecting other bytes in this flash word
void ap3_EEPROM::write(uint16_t eepromLocation, uint8_t dataToWrite)
{
  uint32_t flashLocation = AP3_FLASH_EEPROM_START + eepromLocation;
  writeWordToFlash(flashLocation, (uint32_t)dataToWrite | 0xFFFFFF00);
}

//Read a byte from a given location in "EEPROM"
uint8_t ap3_EEPROM::read(uint16_t eepromLocation)
{
  uint32_t flashLocation = AP3_FLASH_EEPROM_START + eepromLocation;
  return (*(uint8_t *)flashLocation);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//Get method is overloaded with the following variable types
//char, byte, int, unsigned int, long, unsigned long, float, double?

void ap3_EEPROM::get(uint16_t eepromLocation, uint8_t &dataToGet)
{
  dataToGet = *(uint8_t *)(AP3_FLASH_EEPROM_START + eepromLocation);
}
void ap3_EEPROM::get(uint16_t eepromLocation, uint16_t &dataToGet)
{
  dataToGet = *(uint16_t *)(AP3_FLASH_EEPROM_START + eepromLocation);
}
void ap3_EEPROM::get(uint16_t eepromLocation, int16_t &dataToGet)
{
  dataToGet = *(int16_t *)(AP3_FLASH_EEPROM_START + eepromLocation);
}
void ap3_EEPROM::get(uint16_t eepromLocation, int &dataToGet)
{
  dataToGet = *(int *)(AP3_FLASH_EEPROM_START + eepromLocation);
}
void ap3_EEPROM::get(uint16_t eepromLocation, unsigned int &dataToGet)
{
  dataToGet = *(unsigned int *)(AP3_FLASH_EEPROM_START + eepromLocation);
}
void ap3_EEPROM::get(uint16_t eepromLocation, int32_t &dataToGet)
{
  dataToGet = *(int32_t *)(AP3_FLASH_EEPROM_START + eepromLocation);
}
void ap3_EEPROM::get(uint16_t eepromLocation, uint32_t &dataToGet)
{
  dataToGet = *(uint32_t *)(AP3_FLASH_EEPROM_START + eepromLocation);
}
void ap3_EEPROM::get(uint16_t eepromLocation, float &dataToGet)
{
  union {
    float f;
    uint32_t b;
  } temp;
  temp.b = *(uint32_t *)(AP3_FLASH_EEPROM_START + eepromLocation);

  dataToGet = temp.f;
}

void ap3_EEPROM::get(uint16_t eepromLocation, double &dataToGet)
{
  union {
    double lf;
    uint32_t b[2];
  } temp;
  temp.b[1] = *(uint32_t *)(AP3_FLASH_EEPROM_START + eepromLocation);           //LSB;
  temp.b[0] = *(uint32_t *)(AP3_FLASH_EEPROM_START + eepromLocation + 4) << 32; //MSB;
  dataToGet = temp.lf;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//Put method is overloaded with the following variable types
//char, byte, int, unsigned int, long, unsigned long, float, double?

void ap3_EEPROM::put(uint16_t eepromLocation, uint8_t dataToWrite)
{
  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (uint32_t)dataToWrite | 0xFFFFFF00);
}
void ap3_EEPROM::put(uint16_t eepromLocation, uint16_t dataToWrite)
{
  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (uint32_t)dataToWrite | 0xFFFF0000);
}
void ap3_EEPROM::put(uint16_t eepromLocation, int16_t dataToWrite)
{
  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (uint32_t)dataToWrite | 0xFFFF0000);
}
void ap3_EEPROM::put(uint16_t eepromLocation, int dataToWrite) //ints are 32 bit on M4F
{
  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (uint32_t)dataToWrite);
}
void ap3_EEPROM::put(uint16_t eepromLocation, unsigned int dataToWrite) //ints are 32 bit on M4F
{
  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (uint32_t)dataToWrite);
}
void ap3_EEPROM::put(uint16_t eepromLocation, int32_t dataToWrite)
{
  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (int32_t)dataToWrite);
}
void ap3_EEPROM::put(uint16_t eepromLocation, uint32_t dataToWrite)
{
  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (uint32_t)dataToWrite);
}
void ap3_EEPROM::put(uint16_t eepromLocation, float dataToWrite)
{
  union {
    float f;
    uint32_t b;
  } temp;
  temp.f = dataToWrite;

  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (uint32_t)temp.b);
}

void ap3_EEPROM::put(uint16_t eepromLocation, double dataToWrite) //64 bits
{
  union {
    double lf;
    uint32_t b[2];
  } temp;
  temp.lf = dataToWrite;

  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation), (uint32_t)temp.b[1]);     //LSB
  writeWordToFlash((AP3_FLASH_EEPROM_START + eepromLocation + 4), (uint32_t)temp.b[0]); //MSB
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//The update functions simply call the put functions
//Put automatically checks to see if a spot needs updating
void ap3_EEPROM::update(uint16_t eepromLocation, uint8_t dataToWrite)
{
  put(eepromLocation, dataToWrite);
}
void ap3_EEPROM::update(uint16_t eepromLocation, uint16_t dataToWrite)
{
  put(eepromLocation, dataToWrite);
}
void ap3_EEPROM::update(uint16_t eepromLocation, int16_t dataToWrite)
{
  put(eepromLocation, dataToWrite);
}
void ap3_EEPROM::update(uint16_t eepromLocation, int dataToWrite) //ints are 32 bit on M4F
{
  put(eepromLocation, dataToWrite);
}
void ap3_EEPROM::update(uint16_t eepromLocation, unsigned int dataToWrite) //ints are 32 bit on M4F
{
  put(eepromLocation, dataToWrite);
}
void ap3_EEPROM::update(uint16_t eepromLocation, int32_t dataToWrite)
{
  put(eepromLocation, dataToWrite);
}
void ap3_EEPROM::update(uint16_t eepromLocation, uint32_t dataToWrite)
{
  put(eepromLocation, dataToWrite);
}
void ap3_EEPROM::update(uint16_t eepromLocation, float dataToWrite)
{
  put(eepromLocation, dataToWrite);
}
void ap3_EEPROM::update(uint16_t eepromLocation, double dataToWrite) //64 bits
{
  put(eepromLocation, dataToWrite);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

uint16_t ap3_EEPROM::length()
{
  return (AP3_FLASH_EEPROM_SIZE);
}

//Erase 8k page encapsulating the EEPROM section
void ap3_EEPROM::erase()
{
  am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                          AM_HAL_FLASH_ADDR2INST(AP3_FLASH_EEPROM_START),
                          AM_HAL_FLASH_ADDR2PAGE(AP3_FLASH_EEPROM_START));
}

//This is the main helper function
//Reprogram a given location with 32-bits
//Flash is written to in words at locations that are %4=0
//Span words if necessary
//1) Make copy of current flash contents into SRAM
//2) Turn user's requested spot into 0xFFs
//3) Erase flash page (8k)
//4) Write SRAM back into flash
//5) Write user's data onto the spot with recently created 0xFFs
//Note - this code assumes EEPROM temp space is contained in one page
void ap3_EEPROM::writeWordToFlash(uint32_t flashLocation, uint32_t dataToWrite)
{
  //Error check
  if (flashLocation >= AP3_FLASH_EEPROM_START + AP3_FLASH_EEPROM_SIZE)
  {
    return;
  }
  if (flashLocation < AP3_FLASH_EEPROM_START)
  {
    return;
  }

  //Check to see if location needs updating
  if (*(uint32_t *)(flashLocation) == dataToWrite)
  {
    return;
  }

  //First we have to read the contents of current "EEPROM" to SRAM
  uint32_t tempContents[AP3_FLASH_EEPROM_SIZE / 4];
  uint16_t spot = 0;
  for (uint16_t x = 0; x < AP3_FLASH_EEPROM_SIZE; x += 4)
  {
    tempContents[spot++] = *(uint32_t *)(AP3_FLASH_EEPROM_START + x);
  }

  //Then we erase an 8K page
  am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                          AM_HAL_FLASH_ADDR2INST(flashLocation),
                          AM_HAL_FLASH_ADDR2PAGE(flashLocation));

  //Zero out this word(s)
  uint8_t byteOffset = (flashLocation % 4);
  uint16_t wordLocation = (flashLocation - AP3_FLASH_EEPROM_START) / 4;

  //Mask in the new data into the array
  if (byteOffset == 0)
  {
    //Easy - update this word with new word
    tempContents[wordLocation] = dataToWrite;
  }
  else
  {
    //Clear the upper bytes of the first word to 0s
    tempContents[wordLocation] &= ~(0xFFFFFFFF << (byteOffset * 8));

    //Clear the lower bytes of the second word to 0s
    tempContents[wordLocation + 1] &= ~(0xFFFFFFFF >> ((4 - byteOffset) * 8));

    //OR in upper bytes of this word with new data
    uint32_t dataToWriteFirstWord = dataToWrite << (byteOffset * 8);

    //OR in the lower bytes of the following word with new data
    uint32_t dataToWriteSecondWord = dataToWrite >> ((4 - byteOffset) * 8);

    tempContents[wordLocation] |= dataToWriteFirstWord;
    tempContents[wordLocation + 1] |= dataToWriteSecondWord;
  }

  //Then we write the contents of the array back
  am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                            tempContents,
                            (uint32_t *)AP3_FLASH_EEPROM_START,
                            AP3_FLASH_EEPROM_SIZE);
}

ap3_EEPROM EEPROM;