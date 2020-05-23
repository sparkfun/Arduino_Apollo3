/* 
  Author: Adam Garbo
  Created: May 23rd, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to set the RTC using UNIX Epoch time.
  
  Most SparkFun Artemis boards have the necessary external 32kHz crystal to
  enable the RTC. If you are using the Artemis module bare you will either
  need an external 32kHz xtal or use the internal LFRC. Read the datasheet
  section 12.1 for more information.

*/

#include "RTC.h" //Include RTC library included with the Aruino_Apollo3 core
APM3_RTC myRTC; //Create instance of RTC class

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Set UNIX Epoch Time Example");

  // Set the RTC time using UNIX Epoch time
  myRTC.setEpoch(1590240780); // E.g. Saturday, May 23, 2020 1:33:00 PM
}

void loop()
{
  Serial.print("Epoch time: "); Serial.println(myRTC.getEpoch());
  Serial.print("Timestamp: "); printDateTime();

  delay(1000);
}

// Print the RTC's current date and time
void printDateTime()
{
  myRTC.getTime();
  char dateTime[20];
  sprintf(dateTime, "20%02d-%02d-%02d %02d:%02d:%02d",
          myRTC.year, myRTC.month, myRTC.dayOfMonth,
          myRTC.hour, myRTC.minute, myRTC.seconds);
  Serial.println(dateTime);
}
