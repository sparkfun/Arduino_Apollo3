/*
  Author: Nathan Seidle and stephenf7072
  Created: January 28th, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example test the internal HAL to make sure the days advance correctly.
*/

#include "RTC.h"
APM3_RTC myRTC; // Create instance of RTC class

int previousDay;

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("Artemis RTC Testing");

  // Manually set RTC date and time to the start of 2020 so that myRTC contains valid times
  myRTC.setTime(23, 59, 59, 0, 1, 1, 20); // Set to 1 second before midnight Jan 1
}

void loop()
{
  myRTC.setTime(23, 59, 59, 99, myRTC.dayOfMonth, myRTC.month, myRTC.year); // Manually set RTC 1/100th of a second from the next day
  previousDay = myRTC.weekday;
  delay(11); //Allow us to roll from midnight the night before to the new day

  printArtemisTime();
}

void printArtemisTime()
{
  char buf[50];
  char weekdayBuf[4];

  myRTC.getTime();
  int i = myRTC.weekday + 1;
  switch (i)
  {
  case (1):
    strcpy(weekdayBuf, "Sun");
    break;
  case (2):
    strcpy(weekdayBuf, "Mon");
    break;
  case (3):
    strcpy(weekdayBuf, "Tue");
    break;
  case (4):
    strcpy(weekdayBuf, "Wed");
    break;
  case (5):
    strcpy(weekdayBuf, "Thu");
    break;
  case (6):
    strcpy(weekdayBuf, "Fri");
    break;
  case (7):
    strcpy(weekdayBuf, "Sat");
    break;

  default:
    strcpy(weekdayBuf, "???");
    break;
  }

  sprintf(buf, "%02d-%02d-%02d (%s) %02d:%02d:%02d.%02d", myRTC.year, myRTC.month, myRTC.dayOfMonth, weekdayBuf, myRTC.hour, myRTC.minute, myRTC.seconds, myRTC.hundredths);
  Serial.print(buf);

  //Move the previous day forward one day and make sure it matches today
  if ((previousDay + 1) % 7 != myRTC.weekday)
  {
    Serial.printf(" Error! previousDay: %d today: %d\n", previousDay, myRTC.weekday);
    while (1)
      ;
  }

  Serial.println();
}