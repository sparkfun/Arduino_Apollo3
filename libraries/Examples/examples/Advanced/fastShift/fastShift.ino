/* 
  fastShiftIn() / fastShiftOut()
  By: Nathan Seidle
  SparkFun Electronics
  Date: July 8th, 2019
  License: This code is public domain.

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15376

  This example shows how use the new fastShiftOut/In functions.
*/

//Can be any GPIO
#define CLOCK_PIN 14
#define DATA_PIN 27

void setup()
{
  Serial.begin(9600);
  Serial.println("SparkFun Fast Shiftout Example");

  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  //enableBurstMode(); //Optional. Go to 96MHz. Roughly doubles the speed of shiftOut and fastShiftOut

  //This is the standard shiftOut() method
//    while (1)
//    {
//      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, 0x0F); //Without BurstMode shiftOut runs at ~610kHz
//    }

  //Example of new fastShiftOut() method

  //Configure the pins that are to be used for Fast GPIO.
  enableFastShift(DATA_PIN, CLOCK_PIN);
  while (1)
  {
    fastShiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, 0x0F); //Without BurstMode fastShiftOut runs at 4MHz
    //byte incoming = fastShiftIn(DATA_PIN, CLOCK_PIN, LSBFIRST); //Without BurstMode fastShiftIn runs at 3.5MHz
  }
}

void loop()
{

}


