/*
  Author: Adam Garbo and Nathan Seidle
  Created: June 3rd, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to read and set the RTC alarms.

  It is necessary to first set the RTC alarm date and time and then specify
  the alarm mode, which determines which date and time values will be used
  for comparison when generating an alarm interrupt.

  The code is configured so that the RTC alarm will trigger every minute.
  The RTC interrupt service routine will set an alarm flag each time the
  alarm triggers and the RTC date and time will printed to the Serial Monitor.
*/

#include "RTC.h" // Include RTC library included with the Aruino_Apollo3 core

volatile bool alarmFlag = false;

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Set Alarm Example");

  // // Easily set RTC using the system __DATE__ and __TIME__ macros from compiler
  // RTC.setToCompilerTime();

  // Manually set RTC date and time
  RTC.setTime(0, 50, 59, 12, 3, 6, 20); // 12:59:50.000, June 3rd, 2020 (hund, ss, mm, hh, dd, mm, yy)

  // Set the RTC's alarm
  RTC.setAlarm(0, 0, 0, 13, 3, 6); // 13:00:00.000, June 3rd (hund, ss, mm, hh, dd, mm). Note: No year alarm register
  
  // Set the RTC alarm mode
  /*
    0: Alarm interrupt disabled
    1: Alarm match every year   (hundredths, seconds, minutes, hour, day, month)
    2: Alarm match every month  (hundredths, seconds, minutes, hours, day)
    3: Alarm match every week   (hundredths, seconds, minutes, hours, weekday)
    4: Alarm match every day    (hundredths, seconds, minute, hours)
    5: Alarm match every hour   (hundredths, seconds, minutes)
    6: Alarm match every minute (hundredths, seconds)
    7: Alarm match every second (hundredths)
  */
  RTC.setAlarmMode(6); // Set the RTC alarm to match on minutes rollover
  RTC.attachInterrupt(); // Attach RTC alarm interrupt

  // Print the RTC's alarm date and time
  Serial.print("Next alarm: "); printAlarm();
}

void loop()
{
  // Check if alarm flag was set
  if (alarmFlag == true)
  {
    // Print date and time of RTC alarm trigger
    Serial.print("Alarm triggered: "); printDateTime();

    // Clear alarm flag
    alarmFlag = false;
  }

  // Print RTC's date and time while waiting for alarm
  static uint32_t nextPrint = 0;
  if(millis() > nextPrint){
    printDateTime();
    nextPrint = millis() + 1000;
  }
}

// Print the RTC's current date and time
void printDateTime()
{
  RTC.getTime();
  char dateTimeBuffer[25];
  sprintf(dateTimeBuffer, "20%02d-%02d-%02d %02d:%02d:%02d.%03d",
          RTC.year, RTC.month, RTC.dayOfMonth,
          RTC.hour, RTC.minute, RTC.seconds, RTC.hundredths);
  Serial.println(dateTimeBuffer);
}

// Print the RTC's alarm
void printAlarm()
{
  RTC.getAlarm();
  char alarmBuffer[25];
  sprintf(alarmBuffer, "2020-%02d-%02d %02d:%02d:%02d.%03d",
          RTC.alarmMonth, RTC.alarmDayOfMonth,
          RTC.alarmHour, RTC.alarmMinute,
          RTC.alarmSeconds, RTC.alarmHundredths);
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
