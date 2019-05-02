/*
  Verify the basic function of all GPIOs
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 2nd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This blinks each IO pin alternatively (no neighbors) to verify we have digitalWrite
  control of every pin
  
  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Edge: https://www.sparkfun.com/products/15170

  Hardware Connections:
  Connect an Edge via FTDI programmer
  Upload code
  Verify GPIOs are going high/low every 2 seconds

  D2 on Blackboard is GP44 on Apollo3
  D4 = GP38
  SDA = GP40
  SCL = GP39
  BOOT = GP47
  RXI-0 = GP49
  TXO-0 = GP48
  PDMCLK = GP10
  D13 = GP5
  D11 = GP7
  D12 = GP6
  D3 = GP4
  D5 = GP27
  D6 = GP22
  D7 = GP35
  D8 = GP32
  RXI-1 = TX1 is GP24
  TXO-1 = RX1 is GP25

  D9 = GP28
  D10 = GP26
  A0 = GP12
  A1 = GP13
  A2 = GP31
  A3 = GP33
  A4 = GP11
  A5 = GP29
  D14 = GP43
  D15 = GP42
  PMDATA = GP36
  D16 = GP37
  
  
 */

#define testPin 29 //Should toggle A5

#define LED 5 //Should toggle D13

//Fail:
//#define testPin 35 //Should toggle D7 - multi fail
//#define testPin 32 //Should toggle D8 - multi fail
//#define testPin 31 //Should toggle A2

//Good:
//#define testPin 12 //Should toggle A0
//#define testPin 13 //Should toggle A1
//#define testPin 33 //Should toggle A3
//#define testPin 11 //Should toggle A4
//#define testPin 29 //Should toggle A5

//#define testPin 44 //Should toggle D2
//#define testPin 4 //Should toggle D3
//#define testPin 38 //Should toggle D4
//#define testPin 27 //Should toggle D5
//#define testPin 22 //Should toggle D6

//#define testPin 28 //Should toggle D9
//#define testPin 26 //Should toggle D10

//#define testPin 7 //Should toggle D11
//#define testPin 6 //Should toggle D12
//#define testPin 5 //Should toggle D13 - fail on one board

//#define testPin 40 //Should toggle SDA
//#define testPin 39 //Should toggle SCL

//#define testPin 47 //Should toggle BOOT - Goes to 3V very slowly

//#define testPin 49 //Should toggle RX0
//#define testPin 48 //Should toggle TX0
//#define testPin 24 //Should toggle RX1
//#define testPin 25 //Should toggle TX1


void setup() {
  Serial.begin(9600);
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
