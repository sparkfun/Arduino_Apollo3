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

#ifndef _APOLLO3_LIBRARIES_EEPROM_H_
#define _APOLLO3_LIBRARIES_EEPROM_H_

#include "Arduino.h"

#define EEPROM_DEFAULT_SRAM_USAGE (1024)

typedef struct _eeprom_config_t {
    mbed::bd_size_t sram_bytes = EEPROM_DEFAULT_SRAM_USAGE;
} eeprom_config_t;

class EEPROMClass : protected FlashIAPBlockDevice {
private:
    eeprom_config_t _cfg;

protected:
public:
    EEPROMClass(void);
    EEPROMClass(uint32_t address, uint32_t size);

    void config(eeprom_config_t cfg){
        _cfg = cfg;
    }
    void configSramUsage(mbed::bd_size_t bytes){
        _cfg.sram_bytes = bytes;
    }

    void read(int idx, uint8_t* data, uint32_t size){
        FlashIAPBlockDevice::read(data, idx, size);
    }
    uint8_t read(int idx){
        uint8_t val = 0x00;
        read(&val, idx, 1);
        return val;
    }
    void write(int idx, uint8_t* data, uint32_t size){
        mbed::bd_size_t scratch_size = (_cfg.sram_bytes+3)/4;
        uint32_t scratch[scratch_size];
        FlashIAPBlockDevice::read((uint8_t*)scratch, 0, _cfg.sram_bytes);   // keep all of flash in sram in case we need to erase
        if(memcmp((void*)(((uint8_t*)scratch) + idx), data, size)){         // compare desired data (data) to existing information in flash (scratch)
            erase();
            int result = FlashIAPBlockDevice::program((uint8_t*)scratch, 0, 4*scratch_size);
            printf("updating flash. result: %d\n", result);
            return;
        }
        printf("contents already match\n");
    }
    void write(int idx, uint8_t val){
        write(idx, &val, 1);
    }
    void update(int idx, uint8_t val) {
        write(idx, val);
    }
    void erase(void){
        mbed::bd_size_t erased = 0;
        mbed::bd_size_t block_size = get_erase_size();
        do {
            FlashIAPBlockDevice::erase(erased, block_size);
            erased += block_size;
        } while (erased < size());
    }

    template <typename T> T &get(int idx, T &t){
        read((uint8_t*)&t, idx, sizeof(T)/sizeof(uint8_t));
        return t;
    }

    template <typename T> const T &put(int idx, const T &t){
        write(idx, (uint8_t*)&t, sizeof(T)/sizeof(uint8_t));
        return t;
    }

    uint8_t operator[](const int idx){
        return read(idx);
    }
};

extern EEPROMClass EEPROM;

#endif // _APOLLO3_LIBRARIES_EEPROM_H_
