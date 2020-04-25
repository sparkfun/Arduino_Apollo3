/* Author: Nathan Seidle
  Created: Septempter 27th, 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to put the core to sleep for a number of
  milliseconds before waking and printing the current time/date. This
  is helpful for checking power consumption of the core while RTC+CT6 are running.
*/

#include "RTC.h" //Include RTC library included with the Aruino_Apollo3 core
APM3_RTC myRTC; //Create instance of RTC class

uint32_t msToSleep = 2000; //This is the user editable number of ms to sleep between RTC checks
#define TIMER_FREQ 3000000L //Counter/Timer 6 will use the HFRC oscillator of 3MHz
uint32_t sysTicksToSleep = msToSleep * (TIMER_FREQ / 1000);

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Example");

  myRTC.setToCompilerTime(); //Easily set RTC using the system __DATE__ and __TIME__ macros from compiler
  //myRTC.setTime(0, 51, 28, 7, 21, 10, 15); //Manually set RTC back to the future: Oct 21st, 2015 at 7:28.51 AM

  setupWakeTimer();
}

void loop()
{
  myRTC.getTime();

  Serial.printf("It is now ");
  Serial.printf("%d:", myRTC.hour);
  Serial.printf("%02d:", myRTC.minute);
  Serial.printf("%02d.", myRTC.seconds);
  Serial.printf("%02d", myRTC.hundredths);

  Serial.printf(" %02d/", myRTC.month);
  Serial.printf("%02d/", myRTC.dayOfMonth);
  Serial.printf("%02d", myRTC.year);

  Serial.printf(" Day of week: %d =", myRTC.weekday);
  Serial.printf(" %s", myRTC.textWeekday);

  Serial.println();

  am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP); //Sleepy time
}

//We use counter/timer 6 for this example but 0 to 7 are available
//CT 7 is used for Software Serial. All CTs are used for Servo.
void setupWakeTimer()
{
  //Clear compare interrupt
  am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREG); //Use CT6

  am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREG); // Enable C/T G=6

  //Don't change from 3MHz system timer, but enable G timer
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ | AM_HAL_STIMER_CFG_COMPARE_G_ENABLE);

  //Setup ISR to trigger when the number of ms have elapsed
  am_hal_stimer_compare_delta_set(6, sysTicksToSleep);

  //Enable the timer interrupt in the NVIC.
  NVIC_EnableIRQ(STIMER_CMPR6_IRQn);
}

//Called once number of milliseconds has passed
extern "C" void am_stimer_cmpr6_isr(void)
{
  uint32_t ui32Status = am_hal_stimer_int_status_get(false);
  if (ui32Status & AM_HAL_STIMER_INT_COMPAREG)
  {
    am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREG);

    //Reset compare value. ISR will trigger when the number of ms have elapsed
    am_hal_stimer_compare_delta_set(6, sysTicksToSleep);
  }
}
