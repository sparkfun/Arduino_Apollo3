// Author: Owen Lyke
// Created: May 13 2019
// License: MIT. See SparkFun Arduino Apollo3 Project for more information

// This example demonstrates how to use Arduino SPI 
#include "SPI.h"


#define SERIAL_PORT Serial
#define SPI_PORT SPI

#define CS_PIN 2

#define SPI_SPEED 2000000
#define SPI_ORDER MSBFIRST
#define SPI_MODE SPI_MODE3

#define SPI_SPEED_2 1000000
#define SPI_ORDER_2 MSBFIRST
#define SPI_MODE_2 SPI_MODE0

SPISettings mySettings(SPI_SPEED, SPI_ORDER, SPI_MODE);
SPISettings mySettings2(SPI_SPEED_2, SPI_ORDER_2, SPI_MODE_2);

const char* msg = "Hello world!";

void setup() {

  SERIAL_PORT.begin(115200);
  while(!SERIAL_PORT){};

  SERIAL_PORT.println("SparkFun Arduino Apollo3 SPI Example");
  SERIAL_PORT.printf("Compiled on %s, %s\n\n", __DATE__, __TIME__);

  SPI_PORT.begin();

  pinMode(CS_PIN, OUTPUT);
}

void loop()
{
  digitalWrite(CS_PIN, LOW);
  SPI_PORT.beginTransaction(mySettings);
  SPI_PORT.transfer(0xAA);
  SPI_PORT.endTransaction();

  SPI_PORT.beginTransaction(mySettings2);
  SPI_PORT.transferOut((void*)msg, strlen(msg));  
  SPI_PORT.endTransaction();
  digitalWrite(CS_PIN, HIGH);
  delay(1000);           // wait 1 second for next scan
}
