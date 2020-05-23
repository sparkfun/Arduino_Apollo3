/*
  Author: Adam Garbo
  Created: May 23rd, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to read and set RTC alarms.

  Most SparkFun Artemis boards have the necessary external 32kHz crystal to
  enable the RTC. If you are using the Artemis module bare you will either
  need an external 32kHz xtal or use the internal LFRC. Read the datasheet
  section 12.1 for more information.
*/

#include "RTC.h" //Include RTC library included with the Aruino_Apollo3 core
APM3_RTC myRTC; //Create instance of RTC class

volatile bool alarmFlag = false;

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Set Alarm Example");

  //myRTC.setToCompilerTime(); // Easily set RTC using the system __DATE__ and __TIME__ macros from compiler
  myRTC.setTime(0, 50, 59, 12, 23, 5, 20); // Manually set RTC date and time (hund, ss, mm, hh, dd, mm, yy)

  Serial.print("Initial alarm: "); printAlarm(); // Initial alarm should read all zeroes

  // Set the RTC's alarm
  myRTC.setAlarm(0, 0, 0, 13, 23, 5); // Set alarm (hund, ss, mm, hh, dd, mm). Note: No year alarm register

  // Set the RTC alarm mode
  /*
    0: Alarm interrupt disabled
    1: Alarm match every year
    2: Alarm match every month
    3: Alarm match every week
    4: Alarm match every day
    5: Alarm match every hour
    6: Alarm match every minute
    7: Alarm match every second
    8: Alarm match every 10th second
    9: Alarm match every 100th second
  */
  myRTC.setAlarmMode(6); // Set the RTC alarm to match on minutes rollover
  myRTC.attachInterrupt(); // Attach RTC alarm interrupt
  
  Serial.print("Next alarm: "); printAlarm(); // Print current alarm date and time
}

void loop()
{
  if (alarmFlag == true)
  {
    Serial.print("Alarm triggered: "); printDateTime();
    alarmFlag = false;
  }

  printDateTime();
  delay(1000);
}

// Print the RTC's current date and time
void printDateTime()
{
  myRTC.getTime();
  char dateTimeBuffer[25];
  sprintf(dateTimeBuffer, "20%02d-%02d-%02d %02d:%02d:%02d.%03d",
          myRTC.year, myRTC.month, myRTC.dayOfMonth,
          myRTC.hour, myRTC.minute, myRTC.seconds, myRTC.hundredths);
  Serial.println(dateTimeBuffer);
}

// Print the RTC's alarm
void printAlarm()
{
  myRTC.getAlarm();
  char alarmBuffer[25];
  sprintf(alarmBuffer, "2020-%02d-%02d %02d:%02d:%02d.%03d",
          myRTC.alarmMonth, myRTC.alarmDayOfMonth,
          myRTC.alarmHour, myRTC.alarmMinute,
          myRTC.alarmSeconds, myRTC.alarmHundredths);
  Serial.println(alarmBuffer);
}

// Interrupt handler for the RTC
extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt.
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Set alarm flag
  alarmFlag = true;
}
