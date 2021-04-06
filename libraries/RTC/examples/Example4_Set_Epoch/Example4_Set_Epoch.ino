/* 
  Author: Adam Garbo and Nathan Seidle
  Created: June 3rd, 2020

  This example demonstrates how to set the RTC using UNIX Epoch time.
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "RTC.h" // Include RTC library included with the Aruino_Apollo3 core

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Set UNIX Epoch Time Example");

  // Set the RTC time using UNIX Epoch time
  rtc.setEpoch(1591185600); // E.g. 12:00:00, June 3rd, 2020 
}

void loop()
{
  // Print UNIX Epoch timestamp
  Serial.print("Epoch time: "); Serial.println(rtc.getEpoch());
  
  // Print RTC's date and time
  Serial.print("Timestamp: "); printDateTime();

  delay(1000);
}

// Print the RTC's current date and time
void printDateTime()
{
  rtc.getTime();
  Serial.printf("20%02d-%02d-%02d %02d:%02d:%02d\n",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);
  Serial.println();
}
