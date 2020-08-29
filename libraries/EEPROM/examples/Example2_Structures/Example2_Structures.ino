/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "EEPROM.h"

#define SERIAL Serial

#define EEPROM_PREFS_IDX  (0)
#define EEPROM_VALID_CODE (0xAB)
#define DEFAULT_VER_MAJOR (0)
#define DEFAULT_VER_MINOR (0)
#define DEFAULT_VER_PATCH (0)

typedef struct {
  uint8_t valid = EEPROM_VALID_CODE;
  uint8_t ver_major = DEFAULT_VER_MAJOR;
  uint8_t ver_minor = DEFAULT_VER_MINOR;
  uint8_t ver_patch = DEFAULT_VER_PATCH;
} preferences_t;

preferences_t prefs;

void setup() {
  SERIAL.begin(115200);
  SERIAL.println("EEPROM Example2_Structures");

  // use EEPROM.get(int index, T type) to retrieve
  // an arbitrary type from flash memory
  EEPROM.get(EEPROM_PREFS_IDX, prefs);

  if(prefs.valid != EEPROM_VALID_CODE){
    SERIAL.println("EEPROM was invalid");
    
    // use EEPROM to store the default structure
    preferences_t default_prefs;
    EEPROM.put(EEPROM_VALID_IDX, default_prefs);
    
    SERIAL.println("EEPROM initialized");
  }

  // verify that the prefs are valid
  EEPROM.get(EEPROM_PREFS_IDX, prefs);
  if(prefs.valid != EEPROM_VALID_CODE){
    SERIAL.println("ERROR");
    while(1){};
  }

  SERIAL.println("EEPROM is valid");

  SERIAL.printf("version: %d.%d.%d\n", refs.major, prefs.minor, prefs.patch);

  SERIAL.printf("\nany characters received over SERIAL will increment the patch version and be stored after power-down\n");
}

void loop() {
  if(SERRIAL.available()){
    while(SERIAL.available()){
      SERIAL.read();
      prefs.patch++;
    }
    EEPROM.put(EEPROM_PREFS_IDX, prefs);
    SERIAL.printf("version: %d.%d.%d\n", refs.major, prefs.minor, prefs.patch);
  }
}
