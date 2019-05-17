// Author: Owen Lyke
// Created: May 13 2019
// License: MIT. See SparkFun Arduino Apollo3 Project for more information

// This example demonstrates how to use Arduino Wire (I2C) by scanning all
// possible I2C addresses and indicating which have responding devices
#include "Wire.h"

// Wire objects use IOMaster objects (which represent the IO Master peripherals on the
// Apollo3) and you can declare your own by simply providing which IOmaster instance
// to use. Like this:

// TwoWire MyWire(2); 

// Because there is no pin multiplexing on the IO master peripherals the instance number
// that you choose directly determines which pads it is connected to. Here's the list:
/*

IO Master 0
  case AP3_IOM_I2C_SCL : *pad = 5; *funcsel = AM_HAL_PIN_5_M0SCL; break;
  case AP3_IOM_I2C_SDA : *pad = 6; *funcsel = AM_HAL_PIN_6_M0SDAWIR3; break;
  case AP3_IOM_SPI_SCLK : *pad = 5; *funcsel = AM_HAL_PIN_5_M0SCK; break;
  case AP3_IOM_SPI_MOSI : *pad = 7; *funcsel = AM_HAL_PIN_7_M0MOSI; break;
  case AP3_IOM_SPI_MISO : *pad = 6; *funcsel = AM_HAL_PIN_6_M0MISO; break;

IO Master 1
  case AP3_IOM_I2C_SCL : *pad = 8; *funcsel = AM_HAL_PIN_8_M1SCL; break;
  case AP3_IOM_I2C_SDA : *pad = 9; *funcsel = AM_HAL_PIN_9_M1SDAWIR3; break;
  case AP3_IOM_SPI_SCLK : *pad = 8; *funcsel = AM_HAL_PIN_8_M1SCK; break;
  case AP3_IOM_SPI_MOSI : *pad = 10; *funcsel = AM_HAL_PIN_10_M1MOSI; break;
  case AP3_IOM_SPI_MISO : *pad = 9; *funcsel = AM_HAL_PIN_9_M1MISO; break;
  
IO Master 2
  case AP3_IOM_I2C_SCL : *pad = 27; *funcsel = AM_HAL_PIN_27_M2SCL; break;
  case AP3_IOM_I2C_SDA : *pad = 25; *funcsel = AM_HAL_PIN_25_M2SDAWIR3; break;
  case AP3_IOM_SPI_SCLK : *pad = 27; *funcsel = AM_HAL_PIN_27_M2SCK; break;
  case AP3_IOM_SPI_MOSI : *pad = 28; *funcsel = AM_HAL_PIN_28_M2MOSI; break;
  case AP3_IOM_SPI_MISO : *pad = 25; *funcsel = AM_HAL_PIN_25_M2MISO; break;
  
IO Master 3  
  case AP3_IOM_I2C_SCL : *pad = 42; *funcsel = AM_HAL_PIN_42_M3SCL; break;
  case AP3_IOM_I2C_SDA : *pad = 43; *funcsel = AM_HAL_PIN_43_M3SDAWIR3; break;
  case AP3_IOM_SPI_SCLK : *pad = 42; *funcsel = AM_HAL_PIN_42_M3SCK; break;
  case AP3_IOM_SPI_MOSI : *pad = 38; *funcsel = AM_HAL_PIN_38_M3MOSI; break;
  case AP3_IOM_SPI_MISO : *pad = 43; *funcsel = AM_HAL_PIN_43_M3MISO; break;

IO Master 4
  case AP3_IOM_I2C_SCL : *pad = 39; *funcsel = AM_HAL_PIN_39_M4SCL; break;
  case AP3_IOM_I2C_SDA : *pad = 40; *funcsel = AM_HAL_PIN_40_M4SDAWIR3; break;
  case AP3_IOM_SPI_SCLK : *pad = 39; *funcsel = AM_HAL_PIN_39_M4SCK; break;
  case AP3_IOM_SPI_MOSI : *pad = 44; *funcsel = AM_HAL_PIN_44_M4MOSI; break;
  case AP3_IOM_SPI_MISO : *pad = 40; *funcsel = AM_HAL_PIN_40_M4MISO; break;

IO Master 5
  case AP3_IOM_I2C_SCL : *pad = 48; *funcsel = AM_HAL_PIN_48_M5SCL; break;
  case AP3_IOM_I2C_SDA : *pad = 49; *funcsel = AM_HAL_PIN_49_M5SDAWIR3; break;
  case AP3_IOM_SPI_SCLK : *pad = 48; *funcsel = AM_HAL_PIN_48_M5SCK; break;
  case AP3_IOM_SPI_MOSI : *pad = 47; *funcsel = AM_HAL_PIN_47_M5MOSI; break;
  case AP3_IOM_SPI_MISO : *pad = 49; *funcsel = AM_HAL_PIN_49_M5MISO; break;

*/

// Lastly, the Variant you use will probably have one or more TwoWire objects declared.
// For example on the Edge board 'Wire' corresponds to the I2C bus that connects to the 
// Qwiic connector (and you can also use 'WireQwiic' to access it) while Wire1 (also 
// 'WireAccel') is connected to the onboard LIS2DH accelerometer. 

#define SERIAL_PORT Serial
//#define WIRE_PORT Wire // Uncomment to scan devices on the Qwiic connector on the Edge board
#define WIRE_PORT WireAccel // Use this line to scan for devices on Wire1 (connects to the onboard accelerometer on the Edge board)

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
 
  delay(1000);           // wait 1 second for next scan
}
