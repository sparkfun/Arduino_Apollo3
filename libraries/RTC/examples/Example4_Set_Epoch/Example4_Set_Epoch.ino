/* 
  Author: Adam Garbo and Nathan Seidle
  Created: June 3rd, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to set the RTC using UNIX Epoch time.
*/

#include "RTC.h" // Include RTC library included with the Aruino_Apollo3 core
APM3_RTC myRTC; // Create instance of RTC class

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Set UNIX Epoch Time Example");

  // Set the RTC time using UNIX Epoch time
  myRTC.setEpoch(1591185600); // E.g. 12:00:00, June 3rd, 2020 
}

void loop()
{
  // Print UNIX Epoch timestamp
  Serial.print("Epoch time: "); Serial.println(myRTC.getEpoch());
  
  // Print RTC's date and time
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
