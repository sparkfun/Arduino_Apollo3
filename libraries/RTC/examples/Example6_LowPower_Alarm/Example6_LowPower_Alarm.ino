/*
  Author: Adam Garbo and Nathan Seidle
  Created: June 3rd, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to set an RTC alarm and enter deep sleep.

  The code is configured to set an RTC alarm every minute and enter
  deep sleep between interrupts. The RTC interrupt service routine will
  wake the board and print the date and time upon each alarm interrupt.

  Tested with a SparkFun Edge 2. Confirmed sleep current of 2.5 uA.
*/

#include "RTC.h" // Include RTC library included with the Aruino_Apollo3 core
APM3_RTC myRTC; // Create instance of RTC class

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Low-power Alarm Example");

  // Easily set RTC using the system __DATE__ and __TIME__ macros from compiler
  //myRTC.setToCompilerTime();

  // Manually set RTC date and time
  myRTC.setTime(12, 59, 50, 0, 3, 6, 20); // 12:59:50.000, June 3rd, 2020 (hund, ss, mm, hh, dd, mm, yy)

  // Set the RTC's alarm
  myRTC.setAlarm(13, 0, 0, 0, 3, 6); // 13:00:00.000, June 3rd (hund, ss, mm, hh, dd, mm). Note: No year alarm register

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
  myRTC.setAlarmMode(6); // Set the RTC alarm to match on minutes rollover
  myRTC.attachInterrupt(); // Attach RTC alarm interrupt
}

void loop()
{
  // Print date and time of RTC alarm trigger
  Serial.print("Alarm interrupt: "); printDateTime();

  // Enter deep sleep and await RTC alarm interrupt
  goToSleep();
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

// Power down gracefully
void goToSleep()
{
  // Disable UART
  Serial.end();

  // Disable ADC
  power_adc_disable();

  // Force the peripherals off
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM1);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM2);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM3);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM4);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM5);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART1);

  // Disable all pads
  for (int x = 0 ; x < 50 ; x++)
    am_hal_gpio_pinconfig(x, g_AM_HAL_GPIO_DISABLE);

  //Power down Flash, SRAM, cache
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_CACHE); // Turn off CACHE
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_FLASH_512K); // Turn off everything but lower 512k
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_SRAM_64K_DTCM); // Turn off everything but lower 64k
  //am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL); //Turn off all memory (doesn't recover)

  // Keep the 32kHz clock running for RTC
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);

  am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP); // Sleep forever

  // And we're back!
  wakeUp();
}

// Power up gracefully
void wakeUp()
{
  // Power up SRAM, turn on entire Flash
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_MAX);

  // Go back to using the main clock
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

  // Enable ADC
  ap3_adc_setup();

  // Enable Serial
  Serial.begin(115200);
}

// Interrupt handler for the RTC
extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);
}
