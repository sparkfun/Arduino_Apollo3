/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "EEPROM.h"

#define SERIAL Serial

#define EEPROM_VALID_IDX  (0)
#define EEPROM_VALID_CODE (0xAB)

void setup() {
  SERIAL.begin(115200);
  SERIAL.println("EEPROM Example1_PutGet");
  EEPROM.init();

  // use EEPROM.get(int index, T type) to retrieve
  // an arbitrary type from flash memory
  uint8_t eeprom_valid;
  EEPROM.get(EEPROM_VALID_IDX, eeprom_valid);

  if(eeprom_valid != EEPROM_VALID_CODE){
    SERIAL.println("EEPROM was invalid");
    
    // use EEPROM.put(int index, T type) to store 
    // a variable in psuedo-eeprom flash memory
    // use a variable with a type so that EEPROM
    // knows how much memory to use
    uint8_t code_to_store = EEPROM_VALID_CODE;
    EEPROM.put(EEPROM_VALID_IDX, code_to_store);
    
    SERIAL.println("EEPROM initialized");
  }

  EEPROM.get(EEPROM_VALID_IDX, eeprom_valid);
  if(eeprom_valid != EEPROM_VALID_CODE){
    SERIAL.println("ERROR");
    while(1){};
  }

  SERIAL.println("EEPROM is valid");
}

void loop() {
}
