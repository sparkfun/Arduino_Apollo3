/*
  Verify the basic function of all GPIOs
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 2nd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This blinks alternating pins to verify we have digitalWrite
  control of every pin (and no shorts between pins). Useful for initial hardware testing but
  otherwise a pretty boring sketch.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Edge: https://www.sparkfun.com/products/15170

  Hardware Connections:
  Connect an Edge via serial programmer
  Upload code
  Verify GPIOs are going high/low every 2 seconds
*/

byte pinsToToggle = 0;

void setup() {
  Serial.begin(9600);

#ifdef ARDUINO_AM_AP3_SFE_BB_ARTEMIS
  Serial.println("SparkFun BlackBoard Artemis");
  pinsToToggle = 30;
#elif AM_AP3_SFE_BB_ARTEMIS_NANO 
  Serial.println("SparkFun BlackBoard Artemis Nano");
  pinsToToggle = 20;
#elif AM_AP3_SFE_BB_ARTEMIS_MEGA 
  Serial.println("SparkFun BlackBoard Artemis Mega");
  pinsToToggle = 50;
#else
  Serial.println("Unknown board.");
  pinsToToggle = 10;
#endif

  Serial.printf("Toggling %d pins\n\r", pinsToToggle); //My goodness it's nice to have printf again
  
  Serial.println("pinMode will override the serial configuration so no more printing...");

  for(int x = 0 ; x < pinsToToggle ; x++)
  {
    pinMode(x, OUTPUT);
  }
}

void loop() {
  for(int x = 0 ; x < pinsToToggle ; x++)
  {
    if(x % 2 == 0) digitalWrite(x, LOW);
    else digitalWrite(x, HIGH);
  }
  delay(2000);

  for(int x = 0 ; x < pinsToToggle ; x++)
  {
    if(x % 2 == 0) digitalWrite(x, HIGH);
    else digitalWrite(x, LOW);
  }
  delay(2000);
}
