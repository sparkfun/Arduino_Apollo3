// Author: Owen Lyke
// Created: May 13 2019
// License: MIT. See SparkFun Arduino Apollo3 Project for more information

// This example demonstrates how to use Arduino Wire (I2C) by scanning all
// possible I2C addresses and indicating which have responding devices

#include "Wire.h"

#define SERIAL_PORT Serial
#define WIRE_PORT Wire

void setup() {

  SERIAL_PORT.begin(115200);
  while(!SERIAL_PORT){};

  SERIAL_PORT.println("SparkFun Arduino Apollo3 Wire Example (I2C Scanner)");
  SERIAL_PORT.printf("Compiled on %s, %s\n\n", __DATE__, __TIME__);

  WIRE_PORT.begin();
}

void loop()
{
  uint8_t error, address;
  int nDevices;
 
  SERIAL_PORT.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE_PORT.beginTransmission(address);
    error = WIRE_PORT.endTransmission();
 
    if (error == 0)
    {
      SERIAL_PORT.print("I2C device found at address 0x");
      if (address<16)
        SERIAL_PORT.print("0");
      SERIAL_PORT.print(address,HEX);
      SERIAL_PORT.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      SERIAL_PORT.print("Unknown error at address 0x");
      if (address<16)
        SERIAL_PORT.print("0");
      SERIAL_PORT.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    SERIAL_PORT.println("No I2C devices found\n");
  else
    SERIAL_PORT.println("done\n");
 
  delay(1000);           // wait 5 seconds for next scan
}
