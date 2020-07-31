/*
  Author: Adam Garbo
  Created: July 31st, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates the combined use of the Artemis Watchdog Timer (WDT)
  and real-time clock (RTC).

  Both RTC and WDT interrupts will wake the system, print the date and time,
  and then re-enter deep sleep.

  The code is configured to trigger the RTC alarm every minute and enter
  deep sleep between interrupts. Alarm interupts also restart the WDT and reset
  the watchdog counter.

  If the WDT is not restarted after 75 seconds, a system reset will occur.
*/

#include <RTC.h>
#include <WDT.h>

APM3_RTC rtc;
APM3_WDT wdt;

volatile bool alarmFlag = false; // RTC ISR flag
volatile bool watchdogFlag = false; // Watchdog Timer ISR flag
volatile int watchdogCounter = 0; // Watchdog interrupt counter

void setup()
{
  Serial.begin(115200);

  Serial.println("Artemis Watchdog Low Power Example");

  // Set the RTC's alarm
  rtc.setAlarm(0, 0, 0, 0, 0, 0); // Set alarm (hund, ss, mm, hh, dd, mm)
  rtc.setAlarmMode(6); // Set the RTC alarm to trigger every minute
  rtc.attachInterrupt(); // Attach RTC alarm interrupt

  // Configure the watchdog
  wdt.configure(); // Default interrupt: 5 seconds. Default reset: 15 seconds

  // Enable the watchdog
  wdt.enable();
}

void loop()
{
  // Check for alarm interrupt
  if (alarmFlag)
  {
    Serial.print("Alarm triggered: ");
    printDateTime(); // Print RTC's date and time
    alarmFlag = false;

    wdt.restart(); // "Pet" the dog
    watchdogCounter = 0; // Reset watchdog interrupt counter
  }

  // Check for watchdog interrupt
  if (watchdogFlag)
  {
    Serial.print("Watchdog triggered: ");
    printDateTime(); // Print RTC's date and time
    watchdogFlag = false; // Clear watchdog flag
  }

  goToSleep(); // Enter deep sleep

}

// Print the RTC's current date and time
void printDateTime()
{
  rtc.getTime();
  Serial.printf("20%02d-%02d-%02d %02d:%02d:%02d.%02d\n",
                rtc.year, rtc.month, rtc.dayOfMonth,
                rtc.hour, rtc.minute, rtc.seconds, rtc.hundredths);
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

  alarmFlag = true; // Set alarm flag
}

// Interrupt handler for the watchdog.
extern "C" void am_watchdog_isr(void) {

  // Clear the watchdog interrupt
  wdt.clear();

  // Perform system reset after 15 watchdog interrupts (15 * 5 seconds = 75 seconds)
  if ( watchdogCounter < 20 ) {
    wdt.restart(); // "Pet" the dog
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH); // Visual indication of system reset
  }

  watchdogFlag = true; // Set the watchdog flag
  watchdogCounter++; // Increment watchdog interrupt counter
}
