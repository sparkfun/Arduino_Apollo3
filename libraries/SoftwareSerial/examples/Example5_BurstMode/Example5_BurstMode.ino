/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: June 19 2019

  Feel like supporting open source hardware? Buy a board from SparkFun!
  https://www.sparkfun.com/artemis

  This example shows how to enable burst mode. Although Artemis is running at
  96MHz, software serial operates the same way.

  Hardware Connections:
  Attach a USB to serial converter (https://www.sparkfun.com/products/15096)
  Connect
    GND on SerialBasic <-> GND on Artemis
    RXO on SerialBasic <-> Pin 8 on Artemis
    TXO on SerialBasic <-> Pin 7 on Artemis
  Load this code
  Open Arduino serial monitor at 57600
  Open Terminal window (TeraTerm) at 57600
  Press a button in terminal window, you should see it in Arduino monitor
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <SoftwareSerial.h>
SoftwareSerial mySerial(7, 8); //RX, TX - Any pins can be used

int counter = 0;

void setup() {
  //We set the serial monitor speed high so that we spend less time printing the output
  //and more time checking mySerial.available()
  Serial.begin(57600); 
  Serial.println("Software Serial Example");

  enableBurstMode(); //Go to 96MHz

  mySerial.begin(57600);
}

void loop() {

  mySerial.print("Hi: ");
  mySerial.println(counter++);
  
  while (mySerial.available())
  {
    byte incoming = mySerial.read();
    Serial.write(incoming);
  }

  delay(100); //Small delay between prints so that we can detect incoming chars if any
}
