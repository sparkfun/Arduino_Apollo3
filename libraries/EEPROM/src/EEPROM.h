/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _APOLLO3_LIBRARIES_EEPROM_H_
#define _APOLLO3_LIBRARIES_EEPROM_H_

#include "Arduino.h"
#include "FlashIAPBlockDevice.h"

#define EEPROM_DEFAULT_SRAM_USAGE (1024)

typedef struct _eeprom_config_t {
    mbed::bd_size_t sram_bytes = EEPROM_DEFAULT_SRAM_USAGE;
} eeprom_config_t;

class EEPROMClass : public FlashIAPBlockDevice {
private:
    eeprom_config_t _cfg;

protected:
public:
    EEPROMClass(void);
    EEPROMClass(uint32_t address, uint32_t size);

    void config(eeprom_config_t cfg){
        _cfg = cfg;
    }
    mbed::bd_size_t length(void){
        return(_cfg.sram_bytes);
    }
    void configSramUsage(mbed::bd_size_t bytes){
        _cfg.sram_bytes = bytes;
    }
    void setLength(mbed::bd_size_t bytes){ //Old function to maintain compatibility
        configSramUsage(bytes);
    }

    void read(int idx, uint8_t* data, uint32_t size){
        FlashIAPBlockDevice::read(data, idx, size);
    }
    uint8_t read(int idx){
        uint8_t val = 0x00;
        read(idx, &val, 1);
        return val;
    }
    void write(int idx, uint8_t* data, uint32_t size){
        mbed::bd_size_t scratch_size = _cfg.sram_bytes;
        uint8_t scratch[scratch_size];

        FlashIAPBlockDevice::read((uint8_t*)scratch, 0, _cfg.sram_bytes);   // keep all of flash in sram in case we need to erase

        if(memcmp((void*)(((uint8_t*)scratch) + idx), data, size)){         // compare desired data (data) to existing information in flash (scratch)
		    memcpy(scratch + idx, data, size);
            erase();
            int result = FlashIAPBlockDevice::program((uint8_t*)scratch, 0, scratch_size);
            return;
        }
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
        read(idx,(uint8_t*)&t, sizeof(T)/sizeof(uint8_t));
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
