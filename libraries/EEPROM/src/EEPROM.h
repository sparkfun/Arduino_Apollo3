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

  Flash is 0x00 to 0xFFFFF. We will do EEPROM writes in the last page of flash:
  0xFFFFF - 8192 = 0xFDFFF. Pages start on 8192 bytes so EEPROM section will start
  at 0xFE000;

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

#ifndef _EEPROM_H
#define _EEPROM_H
#include "Arduino.h"

//EEPROM is arbitrarily located at 0xFE000. This will avoid things that use the
//user code space from 0xC000 to 0xFE000.
//The SparkFun Apollo3 linker script has been modified to limit user code space to less than 0xFE000

#define FLASH_EEPROM_START 0xFE000

#if FLASH_EEPROM_START % 8192
Error : EEPROM start address must be divisble by 8192
#endif

//By limiting EEPROM size to 1024, we reduce the amount of SRAM required and
//time needed to mask in individual bytes and words into flash. It can be increased
//to 8096 if needed
#define FLASH_EEPROM_SIZE 1024

        //class TwoWire : public Stream, public IOMaster{}

        class ap3_EEPROM
{
public:
  ap3_EEPROM();

  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //8 bit
  uint8_t read(uint16_t eepromLocation);
  void write(uint16_t eepromLocation, uint8_t dataToWrite);
  void get(uint16_t eepromLocation, uint8_t &dataToGet);

  //16 bit
  void get(uint16_t eepromLocation, uint16_t &dataToGet);
  void get(uint16_t eepromLocation, int16_t &dataToGet);

  //32 bit
  void get(uint16_t eepromLocation, int &dataToGet);
  void get(uint16_t eepromLocation, unsigned int &dataToGet);
  void get(uint16_t eepromLocation, int32_t &dataToGet);
  void get(uint16_t eepromLocation, uint32_t &dataToGet);
  void get(uint16_t eepromLocation, float &dataToGet);

  //64 bit
  void get(uint16_t eepromLocation, double &dataToGet);

  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //8 bit
  void put(uint16_t eepromLocation, uint8_t dataToWrite);

  //16 bit
  void put(uint16_t eepromLocation, uint16_t dataToWrite);
  void put(uint16_t eepromLocation, int16_t dataToWrite);

  // 32 bit
  void put(uint16_t eepromLocation, int dataToWrite);
  void put(uint16_t eepromLocation, unsigned int dataToWrite);
  void put(uint16_t eepromLocation, int32_t dataToWrite);
  void put(uint16_t eepromLocation, uint32_t dataToWrite);
  void put(uint16_t eepromLocation, float dataToWrite);

  //64 bit
  void put(uint16_t eepromLocation, double dataToWrite);

  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //8 bit
  void update(uint16_t eepromLocation, uint8_t dataToWrite);

  //16 bit
  void update(uint16_t eepromLocation, uint16_t dataToWrite);
  void update(uint16_t eepromLocation, int16_t dataToWrite);

  // 32 bit
  void update(uint16_t eepromLocation, int dataToWrite);
  void update(uint16_t eepromLocation, unsigned int dataToWrite);
  void update(uint16_t eepromLocation, int32_t dataToWrite);
  void update(uint16_t eepromLocation, uint32_t dataToWrite);
  void update(uint16_t eepromLocation, float dataToWrite);

  //64 bit
  void update(uint16_t eepromLocation, double dataToWrite);

  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  void erase(); //Erase entire EEPROM

private:
  void writeWordToFlash(uint32_t flashLocation, uint32_t dataToWrite);
};

extern ap3_EEPROM EEPROM;

#endif
