/*
  This example is based on the Ambiq SDK EVB2 RTC example.
*/

#include "RTC.h"
#include <time.h>

am_hal_rtc_time_t hal_time;
am_hal_rtc_time_t alm_time;

#define EPOCH_TIME 946684800 // UNIX Epoch time = 2000-01-01 00:00:00

//String arrays to index Days and Months with the values returned by the RTC.
char const *pcWeekday[] =
    {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Invalid day"};

char const *pcMonth[] =
    {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December",
        "Invalid month"};

//Constructor
APM3_RTC::APM3_RTC()
{
  //Enable the XT for the RTC.
  am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_XTAL_START, 0);

  //Select XT for RTC clock source
  am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);

  //Enable the RTC.
  am_hal_rtc_osc_enable();
}


// Note: Order of parameters to change in v2.0.0.
void APM3_RTC::setTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hund, uint8_t dayOfMonth, uint8_t month, uint16_t year)
{
  hal_time.ui32Weekday = am_util_time_computeDayofWeek(2000 + year, month, dayOfMonth); //computeDayofWeek is expecting 1 to 12 months
  hal_time.ui32Century = 0;
  hal_time.ui32Year = year;
  hal_time.ui32Month = month; //HAL is expecting 1 to 12 months
  hal_time.ui32DayOfMonth = dayOfMonth;
  hal_time.ui32Hour = hour;
  hal_time.ui32Minute = min;
  hal_time.ui32Second = sec;
  hal_time.ui32Hundredths = hund;

  am_hal_rtc_time_set(&hal_time); //Initialize the RTC with this date/time
  getTime();
}

//Takes the time from the last build and uses it as the current time
//Works well as an arduino sketch
void APM3_RTC::setToCompilerTime()
{
  //Get the current date/time from the compiler
  //Alternatively, you can set these values manually
  hal_time.ui32Weekday = am_util_time_computeDayofWeek(2000 + toVal(&__DATE__[9]), mthToIndex(&__DATE__[0]) + 1, toVal(&__DATE__[4]));
  hal_time.ui32Century = 0;
  hal_time.ui32Year = toVal(&__DATE__[9]);
  hal_time.ui32Month = mthToIndex(&__DATE__[0]) + 1; //Compiler ouputs months in 0-11.  
  hal_time.ui32DayOfMonth = toVal(&__DATE__[4]);
  hal_time.ui32Hour = toVal(&__TIME__[0]);
  hal_time.ui32Minute = toVal(&__TIME__[3]);
  hal_time.ui32Second = toVal(&__TIME__[6]);
  hal_time.ui32Hundredths = 00;

  am_hal_rtc_time_set(&hal_time); //Initialize the RTC with this date/time
  getTime();
}

void APM3_RTC::setEpoch(uint32_t ts)
{
  if (ts < EPOCH_TIME) {
    ts = EPOCH_TIME;
  }

  struct tm tm;

  time_t t = ts;
  struct tm* tmp = gmtime(&t);
  hal_time.ui32Weekday = 0;
  hal_time.ui32Century = 0;
  hal_time.ui32Year = tmp->tm_year - 100;
  hal_time.ui32Month = tmp->tm_mon + 1;
  hal_time.ui32DayOfMonth = tmp->tm_mday;
  hal_time.ui32Hour = tmp->tm_hour;
  hal_time.ui32Minute = tmp->tm_min;
  hal_time.ui32Second = tmp->tm_sec;
  hal_time.ui32Hundredths = 0;

  am_hal_rtc_time_set(&hal_time); //Initialize the RTC with this date/time
}

void APM3_RTC::getTime()
{
  am_hal_rtc_time_get(&hal_time);

  weekday = hal_time.ui32Weekday;
  textWeekday = pcWeekday[hal_time.ui32Weekday]; //Given a number (day of week) return the string that represents the name
  year = hal_time.ui32Year;
  month = hal_time.ui32Month; //HAL outputs months in 1 to 12 form
  dayOfMonth = hal_time.ui32DayOfMonth;
  hour = hal_time.ui32Hour;
  minute = hal_time.ui32Minute;
  seconds = hal_time.ui32Second;
  hundredths = hal_time.ui32Hundredths;
}

uint32_t APM3_RTC::getEpoch()
{
  am_hal_rtc_time_get(&hal_time);

  struct tm tm;

  tm.tm_isdst = -1;
  tm.tm_yday = 0;
  tm.tm_wday = 0;
  tm.tm_year = hal_time.ui32Year + 100; //Number of years since 1900.
  tm.tm_mon = hal_time.ui32Month - 1; //mktime is expecting 0 to 11 months
  tm.tm_mday = hal_time.ui32DayOfMonth;
  tm.tm_hour = hal_time.ui32Hour;
  tm.tm_min = hal_time.ui32Minute;
  tm.tm_sec = hal_time.ui32Second;

  return mktime(&tm);
}

void APM3_RTC::getAlarm()
{
  am_hal_rtc_alarm_get(&alm_time); //Get the RTC's alarm time
  
  alarmWeekday = alm_time.ui32Weekday;
  alarmMonth = alm_time.ui32Month; //HAL outputs months in 1 to 12 form
  alarmDayOfMonth = alm_time.ui32DayOfMonth;
  alarmHour = alm_time.ui32Hour;
  alarmMinute = alm_time.ui32Minute;
  alarmSeconds = alm_time.ui32Second;
  alarmHundredths = alm_time.ui32Hundredths;
}

// Note: Order of parameters to change in v2.0.0.
void APM3_RTC::setAlarm(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hund, uint8_t dayOfMonth, uint8_t month)
{
  alm_time.ui32Weekday = 0; // WIP
  alm_time.ui32Month = month;
  alm_time.ui32DayOfMonth = dayOfMonth;
  alm_time.ui32Hour = hour;
  alm_time.ui32Minute = min;
  alm_time.ui32Second = sec;
  alm_time.ui32Hundredths = hund;

  am_hal_rtc_alarm_set(&alm_time, AM_HAL_RTC_ALM_RPT_DIS); //Initialize the RTC alarm with this date/time
}

// Sets the RTC alarm repeat interval.
/*
  RTC alarm repeat intervals
  0: AM_HAL_RTC_ALM_RPT_DIS     Alarm interrupt disabled
  1: AM_HAL_RTC_ALM_RPT_YR      Interrupt every year
  2: AM_HAL_RTC_ALM_RPT_MTH     Interrupt every month
  3: AM_HAL_RTC_ALM_RPT_WK      Interrupt every week
  4: AM_HAL_RTC_ALM_RPT_DAY     Interrupt every day
  5: AM_HAL_RTC_ALM_RPT_HR      Interrupt every hour
  6: AM_HAL_RTC_ALM_RPT_MIN     Interrupt every minute
  7: AM_HAL_RTC_ALM_RPT_SEC     Interrupt every second
  8: AM_HAL_RTC_ALM_RPT_10TH    Interrupt every 10th second (unused)
  9: AM_HAL_RTC_ALM_RPT_100TH   Interrupt every 100th second (unused)
*/
void APM3_RTC::setAlarmMode(uint8_t mode)
{
  am_hal_rtc_alarm_interval_set(mode);
}

void APM3_RTC::attachInterrupt()
{
  // Enable the RTC interrupt.
  am_hal_rtc_int_enable(AM_HAL_RTC_INT_ALM);

  // Clear the RTC interrupt.
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Enable RTC interrupts to the NVIC.
  NVIC_EnableIRQ(RTC_IRQn);
}

void APM3_RTC::detachInterrupt()
{
  // Disable RTC interrupts to the NVIC.
  NVIC_DisableIRQ(RTC_IRQn);

  // Disable the RTC interrupt.
  am_hal_rtc_int_disable(AM_HAL_RTC_INT_ALM);
}

// mthToIndex() converts a string indicating a month to an index value.
// The return value is a value 0-12, with 0-11 indicating the month given
// by the string, and 12 indicating that the string is not a month.
int APM3_RTC::mthToIndex(char const *pcMon)
{
  int idx;
  for (idx = 0; idx < 12; idx++)
  {
    if (am_util_string_strnicmp(pcMonth[idx], pcMon, 3) == 0)
      return idx;
  }
  return 12; //Error
}

// toVal() converts a string to an ASCII value.
int APM3_RTC::toVal(char const *pcAsciiStr)
{
  int iRetVal = 0;
  iRetVal += pcAsciiStr[1] - '0';
  iRetVal += pcAsciiStr[0] == ' ' ? 0 : (pcAsciiStr[0] - '0') * 10;
  return iRetVal;
}
