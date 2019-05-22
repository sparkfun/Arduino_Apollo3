/*
  Verify the basic function of all GPIOs
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 2nd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This blinks a given IO pin to verify we have digitalWrite
  control of every pin. 
  
  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Edge: https://www.sparkfun.com/products/15170

  Hardware Connections:
  Connect an Edge via FTDI programmer
  Upload code
  Verify GPIOs are going high/low every 2 seconds
 */

#define testPin 33

#define LED 5 //On board LED is on pin 5

void setup() {
  Serial.begin(115200);
  Serial.println("SparkFun Arduino Apollo3");

  pinMode(testPin, OUTPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  Serial.println("Toggle");

  digitalWrite(testPin, LOW);
  digitalWrite(LED, LOW);
  delay(2000);

  digitalWrite(testPin, HIGH);
  digitalWrite(LED, HIGH);
  delay(2000);
}
