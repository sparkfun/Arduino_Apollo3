/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: June 19 2019

  Feel like supporting open source hardware? Buy a board from SparkFun!
  https://www.sparkfun.com/artemis

  This example shows how to enable multiple software serial ports at 57600bps.
  You can only receive on one pin at a time. Use .listen() to switch between
  RX pins.

  Note: When multiple ports are enabled receiving at 115200bps is no longer
  possible. This is because the receive interrupt has additional overhead
  and cannot run fast enough to correctly capture bits in time. TX at 115200
  is still supported.

  Hardware Connections:
  Attach a USB to serial converter (https://www.sparkfun.com/products/15096)
  Connect
    GND on SerialBasic <-> GND on Artemis
    RXO on SerialBasic <-> Pin 8 on Artemis
    TXO on SerialBasic <-> Pin 7 on Artemis
  Load this code
  Open Arduino serial monitor at 115200
  Open Terminal window (TeraTerm) at 57600
  Press a button in terminal window, you should see it in Arduino monitor
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <SoftwareSerial.h>
SoftwareSerial mySerialA(7, 8); //RX, TX - Any pins can be used
SoftwareSerial mySerialB(2, 3); //RX, TX - Any pins can be used

int counter = 0;

void setup() {
  //We set the serial monitor speed high so that we spend less time printing the output
  Serial.begin(115200);
  Serial.println("Software Serial Example");

  mySerialA.begin(57600);
  mySerialB.begin(9600);

  //We are now using mySerialB because it was the last one to begin. Use .listen() to change ports.
}

void loop() {
  mySerialA.listen();
  mySerialA.print("I'm A: ");
  mySerialA.println(counter);
  mySerialA.flush(); //Wait for TX buffer to get sent out

  mySerialB.listen();
  mySerialB.print("I'm B: ");
  mySerialB.println(counter);
  mySerialB.flush();

  counter++;
}
