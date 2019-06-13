/* Author: Owen Lyke
  Created: May 13 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to use Arduino Wire (I2C) by scanning all
  possible I2C addresses and indicating which have responding devices

  Wire objects use IOMaster objects (which represent the IO Master peripherals on the
  Apollo3) and you can declare your own by simply providing which IOmaster instance
  to use. Like this:

  TwoWire myWire(2); //Starts I2C on pads 27/25

  Because there is no pin multiplexing on the IO master peripherals the instance number
  that you choose directly determines which pads it is connected to. Here's the list:

  IO Master 0
  AP3_IOM_I2C_SCL : pad = 5
  AP3_IOM_I2C_SDA : pad = 6

  IO Master 1
  AP3_IOM_I2C_SCL : pad = 8
  AP3_IOM_I2C_SDA : pad = 9

  IO Master 2
  AP3_IOM_I2C_SCL : pad = 27
  AP3_IOM_I2C_SDA : pad = 25

  IO Master 3
  AP3_IOM_I2C_SCL : pad = 42
  AP3_IOM_I2C_SDA : pad = 43

  IO Master 4
  AP3_IOM_I2C_SCL : pad = 39
  AP3_IOM_I2C_SDA : pad = 40

  IO Master 5
  AP3_IOM_I2C_SCL : pad = 48
  AP3_IOM_I2C_SDA : pad = 49
*/

#include "Wire.h"

// Lastly, the Variant you use will probably have one or more TwoWire objects declared.
// For example on the Edge board 'Wire' corresponds to the I2C bus that connects to the
// Qwiic connector (and you can also use 'WireQwiic' to access it) while Wire1 (also
// 'WireAccel') is connected to the onboard LIS2DH accelerometer.

//Wire should be used to scan devices on the Qwiic connector on the Edge board
//WireAccel should be used to scan for devices on Wire1 (connects to the onboard accelerometer on the Edge board)

void setup() {
  Serial.begin(9600);
  while (!Serial) {}; //Wait for user to open terminal window

  Serial.println("SparkFun Arduino Apollo3 Wire Example (I2C Scanner)");
  Serial.printf("Compiled on %s, %s\n\n", __DATE__, __TIME__);

  Wire.begin();
}

void loop()
{
  uint8_t error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(1000);           // wait 1 second for next scan
}
