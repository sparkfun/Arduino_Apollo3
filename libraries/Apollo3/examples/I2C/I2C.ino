/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

/*

  I2C functionality is accessed via TwoWire objects or their children.
  On the Apollo3 the MbedI2C class inherits from the TwoWire class and 
  provides the actual implemenation based on mbed-os I2C
  https://os.mbed.com/docs/mbed-os/v6.2/apis/i2c.html

  Apollo3 features 6 IOM peripherals that can each be configured as either
  SPI or I2C. Each IOM has only one set of pins that can be used. Check the 
  board schematic or Apollo3 Blue datasheet to determine which IOM
  peripherals are available on your board. There are a few different ways 
  to access these ports:
  - via pre-defined MbedI2C objects like Wire, Wire1 -> Wire5
  - declaring your own MbedI2C object using pins that correspond to the correct IOM

  Once you have an MbedI2C object to work with you can use all the standard 
  Arduino SPI API methods on it
  https://www.arduino.cc/en/reference/wire

  This example will use threads to organize I2C operations based on board
  capabilities. The test function will list I2C devices detected on each I2C port

  Wiring:
  For each applicable I2C port:
  SCL <--> sensor SCL
  SDA <--> sensor SDA
  3V3 <--> sensor 3V3
  GND <--> sensor GND

*/
 
#include "Wire.h"

void testPortI2C(TwoWire &i2c);

// This thread will use the pre-defined SPI object if it exists
#if VARIANT_WIRE_INTFCS > 0
rtos::Thread wire_thread;
void wire_thread_fn( void ){
  Wire.begin();
  while(1){
    testPortI2C(Wire);
    delay(1000);
  }
}
#endif

// This thread will use the pre-defined SPI1 object if it exists
#if VARIANT_WIRE_INTFCS > 1
rtos::Thread wire1_thread;
void wire1_thread_fn( void ){
  delay(100);
  Wire1.begin();
  while(1){
    testPortI2C(Wire1);
    delay(1000);
  }
}
#endif

// This thread will create its own MbedI2C object using IOM pins
// Define your own pins below to try it
//#define mySDA D25
//#define mySCL D27
#if (defined mySDA) && (defined mySCL)
TwoWire myWire(mySDA, mySCL);
rtos::Thread mywire_thread;
void mywire_thread_fn( void ){
  delay(200);
  myWire.begin();
  while(1){
    testPortI2C(myWire);
    delay(1000);
  }
}
#endif

void testPortI2C(TwoWire &i2c){
  Serial.printf("Scanning... (port: 0x%08X), time (ms): %d\n", (uint32_t)&i2c, millis());

  uint8_t detected = 0;
  for(uint8_t addr = 1; addr < 127; addr++ ){
    // use endTransmission to determine if a device is present at address
    i2c.beginTransmission(addr);
    uint8_t retval = i2c.endTransmission();

    if(retval == 0){
      Serial.printf("\t0x%02X detected\n", addr);
      detected++;
    }
  }

  if(!detected){
    Serial.printf("\tNo device detected!\n");
  }

  Serial.println();
}
 
void setup() {
  Serial.begin(115200);
  Serial.println("Apollo3 - I2C");

  pinMode(LED_BUILTIN, OUTPUT);

#if VARIANT_WIRE_INTFCS > 0
  wire_thread.start(wire_thread_fn);
#endif

#if VARIANT_WIRE_INTFCS > 1
  wire1_thread.start(wire1_thread_fn);
#endif

#if (defined mySDA) && (defined mySCL)
   mywire_thread.start(mywire_thread_fn);
#endif   
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
}
