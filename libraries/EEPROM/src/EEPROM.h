/*
  This is a library written for the SparkFun Artemis

  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!
  https://www.sparkfun.com/products/15332
  https://www.sparkfun.com/products/15376
  https://www.sparkfun.com/products/15411
  https://www.sparkfun.com/products/15412

  Pseudo-EEPROM on the Cortex-M4F

  https://github.com/sparkfun/SparkFun_Apollo3

  There is no EEPROM built into the Apollo3 Cortex-M4F but we have 1M of flash
  so we carve out the last 8k of flash for EEPROM space. Pages are erased in
  8k chunks so 8k of EEPROM works.

  Flash is 0x00 to 0xFFFFF. We will do EEPROM writes in the last page of flash:
  0xFFFFF - 8192 = 0xFDFFF. Pages start on 8192 bytes so EEPROM section will start
  at 0xFE000;

  Page erase takes 15ms
  Writing a byte takes 19ms
  Writing a float across two words takes 19ms
  Update (no write) takes ~1ms

  Development environment specifics:
  Arduino IDE 1.8.x

  Original Copyright (c) 2006 David A. Mellis.  All right reserved.
  New version by Christopher Andrews 2015.
  This copy has minor modificatons for use with Teensy, by Paul Stoffregen
  This copy has minor modificatons for use with Artemis, by Nathan Seidle

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  EERef class.

  This object references an EEPROM cell.
  Its purpose is to mimic a typical byte of RAM, however its storage is the EEPROM.
  This class has an overhead of two bytes, similar to storing a pointer to an EEPROM cell.

*/

#ifndef _EEPROM_H
#define _EEPROM_H
#include "Arduino.h"

//EEPROM is arbitrarily located at 0xFE000. This will avoid things that use the
//user code space from 0xC000 to 0xFE000.
//The SparkFun Apollo3 linker script has been modified to limit user code space to less than 0xFE000

#define AP3_FLASH_EEPROM_START 0xFE000
#define AP3_FLASH_PAGE_SIZE 8192
#define AP3_EEPROM_MAX_LENGTH (AP3_FLASH_PAGE_SIZE - 16)

#if AP3_FLASH_EEPROM_START % AP3_FLASH_PAGE_SIZE
Error : EEPROM start address must be divisble by 8192
#endif

        //The size of the EEPROM may be (optionally) user-configured using the EEPROM.setLength()
        //method. Valid values are in the range [0, AP3_EEPROM_MAX_LENGTH]. Reducing the size of
        //EEPROM will cause data stored in the higher addresses to be lost. The default size is
        //set below. See Example2_AllFunctions for a usage example.

        //Operations on psuedo-EEPROM require a read-write-modify cycle on the entire
        //configured memory area because flash pages cannot be partially erased. The
        //larger the memory area the longer this operation will take. Here are some
        //benchmarks:
        //1024 = 19ms update time
        //2048 = 23ms update time
        const uint16_t AP3_DEFAULT_FLASH_EEPROM_SIZE = 1024; //In bytes

uint8_t read(uint16_t eepromLocation);
void write(uint16_t eepromLocation, uint8_t dataToWrite, uint16_t allowedSize);
void writeBlockToEEPROM(uint16_t eepromLocation, const uint8_t *dataToWrite, uint16_t blockSize, uint16_t allowedSize);

struct EERef
{
  EERef(const int index)
      : index(index) {}

  //Access/read members.
  uint8_t operator*() const { return read(index); }
  operator const uint8_t() const { return **this; }

  //Assignment/write members.
  EERef &operator=(const EERef &ref) { return *this = *ref; }
  EERef &operator=(uint8_t in);
  EERef &operator+=(uint8_t in) { return *this = **this + in; }
  EERef &operator-=(uint8_t in) { return *this = **this - in; }
  EERef &operator*=(uint8_t in) { return *this = **this * in; }
  EERef &operator/=(uint8_t in) { return *this = **this / in; }
  EERef &operator^=(uint8_t in) { return *this = **this ^ in; }
  EERef &operator%=(uint8_t in) { return *this = **this % in; }
  EERef &operator&=(uint8_t in) { return *this = **this & in; }
  EERef &operator|=(uint8_t in) { return *this = **this | in; }
  EERef &operator<<=(uint8_t in) { return *this = **this << in; }
  EERef &operator>>=(uint8_t in) { return *this = **this >> in; }

  EERef &update(uint8_t in) { return in != *this ? *this = in : *this; }

  /** Prefix increment/decrement **/
  EERef &operator++() { return *this += 1; }
  EERef &operator--() { return *this -= 1; }

  /** Postfix increment/decrement **/
  uint8_t operator++(int)
  {
    uint8_t ret = **this;
    return ++(*this), ret;
  }

  uint8_t operator--(int)
  {
    uint8_t ret = **this;
    return --(*this), ret;
  }

  int index; //Index of current EEPROM cell.
};

/***
    EEPtr class.

    This object is a bidirectional pointer to EEPROM cells represented by EERef objects.
    Just like a normal pointer type, this can be dereferenced and repositioned using
    increment/decrement operators.
***/

struct EEPtr
{

  EEPtr(const int index)
      : index(index) {}

  operator const int() const { return index; }
  EEPtr &operator=(int in) { return index = in, *this; }

  //Iterator functionality.
  bool operator!=(const EEPtr &ptr) { return index != ptr.index; }
  EERef operator*() { return index; }

  /** Prefix & Postfix increment/decrement **/
  EEPtr &operator++() { return ++index, *this; }
  EEPtr &operator--() { return --index, *this; }
  EEPtr operator++(int) { return index++; }
  EEPtr operator--(int) { return index--; }

  int index; //Index of current EEPROM cell.
};

/***
    EEPROMClass class.

    This object represents the entire EEPROM space.
    It wraps the functionality of EEPtr and EERef into a basic interface.
    This class is also 100% backwards compatible with earlier Arduino core releases.
***/

struct EEPROMClass
{
  //Basic user access methods.
  EERef operator[](const int idx) { return idx; }
  uint8_t read(int idx) { return EERef(idx); }
  void write(int idx, uint8_t val)
  {
    (EERef(idx)) = val;
  }
  void update(int idx, uint8_t val) { EERef(idx).update(val); }
  void erase();

  //STL and C++11 iteration capability.
  EEPtr
  begin()
  {
    return 0x00;
  }
  EEPtr end() { return length(); } //Standards requires this to be the item after the last valid entry. The returned pointer is invalid.
  uint16_t length()
  {
    return allowedSize;
  }
  void setLength(uint16_t length)
  {
    allowedSize = (length <= AP3_EEPROM_MAX_LENGTH) ? length : AP3_EEPROM_MAX_LENGTH;
  }

  //Functionality to 'get' and 'put' objects to and from EEPROM.
  template <typename T>
  T &get(int idx, T &t)
  {
    EEPtr e = idx;
    uint8_t *ptr = (uint8_t *)&t;
    for (int count = sizeof(T); count; --count, ++e)
      *ptr++ = *e;
    return t;
  }

  template <typename T>
  const T &put(int idx, const T &t) //Address, data
  {
    const uint8_t *ptr = (const uint8_t *)&t;

    writeBlockToEEPROM(idx, ptr, sizeof(T), allowedSize); //Address, data, sizeOfData

    return t;
  }

  uint16_t allowedSize = AP3_DEFAULT_FLASH_EEPROM_SIZE;
};

extern EEPROMClass EEPROM;
#endif //_EEPROM_H
