/* 
  Author: Adam Garbo and Nathan Seidle
  Created: June 3rd, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to set the RTC using UNIX Epoch time.
*/

#include "RTC.h" // Include RTC library included with the Aruino_Apollo3 core

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Set UNIX Epoch Time Example");

  // Set the RTC time using UNIX Epoch time
  RTC.setEpoch(1591185600); // E.g. 12:00:00, June 3rd, 2020 
}

void loop()
{
  // Print UNIX Epoch timestamp
  Serial.print("Epoch time: "); Serial.println(RTC.getEpoch());
  
  // Print RTC's date and time
  Serial.print("Timestamp: "); printDateTime();

  delay(1000);
}

// Print the RTC's current date and time
void printDateTime()
{
  RTC.getTime();
  char dateTime[20];
  sprintf(dateTime, "20%02d-%02d-%02d %02d:%02d:%02d",
          RTC.year, RTC.month, RTC.dayOfMonth,
          RTC.hour, RTC.minute, RTC.seconds);
  Serial.println(dateTime);
}
