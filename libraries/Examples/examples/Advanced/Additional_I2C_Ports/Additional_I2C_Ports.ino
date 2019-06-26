/*
  Making more I2C ports on the Artemis
  By: Nathan Seidle
  SparkFun Electronics
  Date: June 21, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to add a 2nd (or 3rd) I2C bus to an Artemis carrier board.

  The key to success is that when the SPIClass or TwoWire objects above are constructed they don't actually 
  change any of the Special Function Registers (SFRs) in the Apollo3. That all happens when you use the .begin()
  method. This way it is acceptable to define as many different TwoWire or SPIClass objects as you want at one
  time using only one IOMaster instance. However it is *not* well-characterized how it will perform if you try 
  to 'begin' more than one. 

  For this example, you'll need:
  BlackBoard Artemis (or any Artemis board): https://www.sparkfun.com/products/15332
  Qwiic Accel: https://www.sparkfun.com/products/14587
  Qwiic Breadboard cable: https://www.sparkfun.com/products/14425

  Hardware Connections:
  Connect the Qwiic cable to the Accel
  Plug the male pins into the following pins on the BlackBoard Artemis:
    Red -> 3.3V
    Black -> GND
    Yellow -> SCK
    Blue -> MISO
*/

#include <Wire.h>

//Create a TwoWire object that uses a given IOMaster
TwoWire myWire(0); //Will use pads 5/6, pins labeled SCK/MISO on BlackBoard Artemis
//TwoWire myWire(1); //Will use pads 8/9
//TwoWire myWire(2); //Will use pads 25/27, pins labeled D6/RX1 on BlackBoard Artemis
//TwoWire myWire(3); //Will use pads 42/43
//TwoWire myWire(4); //Will use pads 39/40, pins labeled SCL/SDA on BlackBoard Artemis
//TwoWire myWire(5); //Will use pads 48/49, pins labeled TX0/RX0 on BlackBoard Artemis

#include "SparkFun_MMA8452Q.h"    // Click here to get the library: http://librarymanager/All#SparkFun_MMA8452Q
MMA8452Q accel;                   // Create instance of the MMA8452 class

void setup() {
  Serial.begin(9600);
  Serial.println("MMA8452Q on non-standard I2C port!");

  myWire.begin();

  //Here's the next trick. You need to pass this Wire port to your library.
  //SparkFun libraries allow any Wire port to be passed in. Other folk's libraries may not do this.
  if (accel.begin(myWire) == false) {
    Serial.println("Not Connected. Please check connections and read the hookup guide.");
    while (1);
  }
}

void loop() {
  if (accel.available()) {      // Wait for new data from accelerometer
    // Acceleration of x, y, and z directions in g units
    Serial.print(accel.getCalculatedX(), 3);
    Serial.print("\t");
    Serial.print(accel.getCalculatedY(), 3);
    Serial.print("\t");
    Serial.print(accel.getCalculatedZ(), 3);
    Serial.println();
  }
}
