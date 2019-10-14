/*
  Reading and writing to flash using the EEPROM library
  By: Nathan Seidle
  SparkFun Electronics
  Date: June 24th, 2019
  This example code is in the public domain.

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15376

  This example shows how to use the EEPROM get and put functions
  to read and write to EEPROM inside Artemis.

  Artemis doesn't actually have EEPROM. Instead we are writing to a protected
  section of flash. 1028 bytes of flash are updated each time a variable is written. 
  This can take up to 30ms.
  
  Page erase takes 15ms
  Write byte takes 30ms - This is much longer than Arduino that takes 3.3ms
  Float write across two words takes 30ms
  Update (no write) takes 1ms
*/

#include <EEPROM.h>

void setup()
{
  Serial.begin(115200);
  Serial.println("EEPROM Examples");

  byte myValue1 = 200;
  EEPROM.put(0, myValue1); //(location, data)
  byte response1;
  EEPROM.get(0, response1);
  Serial.printf("Location %d should be %d: %d\n\r", 0, myValue1, response1);

  int myValue5 = -245000; //Note that ints are 4 bytes on the Artemis
  EEPROM.put(2, myValue5);
  int response5;
  EEPROM.get(2, response5);
  Serial.printf("Location %d should be %d: %d\n\r", 2, myValue5, response5);

  float myValue9 = -7.35;
  EEPROM.put(10, myValue9);
  float response9;
  EEPROM.get(10, response9);
  Serial.printf("Location %d should be %f: %f\n\r", 10, myValue9, response9);
}

void loop()
{
}
