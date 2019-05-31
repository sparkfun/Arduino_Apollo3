// Author: Owen Lyke
// Created: May 13 2019AM_BSP_GPIO_14
// License: MIT. See SparkFun Arduino Apollo3 Project for more information

// This example demonstrates how to use millis() and micros()

#define SERIAL_PORT Serial

void setup() {

  SERIAL_PORT.begin(115200);
  while(!SERIAL_PORT){};

  SERIAL_PORT.println("SparkFun Arduino Apollo3 STimer Example");
  SERIAL_PORT.printf("Compiled on %s, %s\n\n", __DATE__, __TIME__);
}

void loop()
{
  Serial.printf("Sec: %d, millis: %d, micros: %d, systicks: 0x%08X, sysoverflows: 0x%08X\n", seconds(), millis(), micros(), systicks(), sysoverflows());
  delay(1111);
}
