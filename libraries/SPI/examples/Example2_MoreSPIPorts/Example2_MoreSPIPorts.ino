/* Author: Owen Lyke
  Created: May 13 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to use Arduino SPI
*/

#include "SPI.h"

#define CS_PIN 2

#define SPI_SPEED 1000000
#define SPI_ORDER MSBFIRST
#define SPI_MODE SPI_MODE0

SPISettings mySettings(SPI_SPEED, SPI_ORDER, SPI_MODE);

const char *msg = "Hello world!";

//SPIClass SPI(); //This is default and automatically defined on RedBoard/ATP/Nano. Uses pads 5/6/7 (SCK/MISO/MOSI).
SPIClass SPI1(1); //Use IO Master 1 on pads 8/9/10
// SPIClass mySPI(2); //Use IO Master 2 on pads 27/25/28
// SPIClass anotherSPI(3); //Use IO Master 3 on pads 42/43/44
// SPIClass SPI4(4); //Use IO Master 4 on pads 39/40/38
// SPIClass SPI5(5); //Use IO Master 5 on pads 48/49/47

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }; //Wait for user to open terminal window

  Serial.println("SparkFun Arduino Apollo3 SPI Example");
  Serial.printf("Compiled on %s, %s\n\n", __DATE__, __TIME__);

  SPI1.begin();

  pinMode(CS_PIN, OUTPUT);
}

void loop()
{
  digitalWrite(CS_PIN, LOW);
  SPI1.beginTransaction(mySettings);
  SPI1.transfer(0xAA);
  SPI1.endTransaction();

  SPI1.beginTransaction(mySettings);
  SPI1.transferOut((void *)msg, strlen(msg));
  SPI1.endTransaction();
  digitalWrite(CS_PIN, HIGH);
  delay(1000);
}