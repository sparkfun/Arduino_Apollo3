/*
  Author: Adam Garbo
  Created: March 26, 2021
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates the combined use of the Artemis Watchdog Timer (WDT)
  and real-time clock (RTC).

  Both RTC and WDT interrupts will wake the system, print the date and time,
  and then re-enter deep sleep.

  The WDT is configured to trigger every 10 seconds. If the WDT is not "pet"
  after 100 seconds, a system reset will be triggered.

  The RTC alarm is configured to trigger every minute and enter deep sleep
  between interrupts. Alarm interuptswill also restart the WDT and reset
  the watchdog interrput counter.
*/

#include "RTC.h"
#include "WDT.h"

volatile bool alarmFlag = false; // RTC ISR flag
volatile bool watchdogFlag = false; // Watchdog Timer ISR flag
volatile int watchdogInterrupt = 0; // Watchdog interrupt counter

void setup()
{
  Serial.begin(115200);

  Serial.println("Artemis Watchdog Low Power Example");

  // Set the RTC time using UNIX Epoch time
  rtc.setEpoch(1596240000); // Saturday, August 1, 2020 00:00:00

  // Set the RTC's alarm
  rtc.setAlarm(0, 0, 0, 0, 0, 0); // Set alarm (hund, ss, mm, hh, dd, mm)
  rtc.setAlarmMode(6); // Set the RTC alarm to trigger every minute
  rtc.attachInterrupt(); // Attach RTC alarm interrupt

  // Configure the watchdog timer
  // See Example2_WDT_Config for more information on how to configure the watchdog
  wdt.configure(WDT_16HZ, 160, 240); // 16 Hz clock, 10-second interrupt period, 15-second reset period

  // Start the watchdog
  wdt.start();
}

void loop()
{
  // Check for alarm interrupt
  if (alarmFlag)
  {
    Serial.print("Alarm interrupt: ");
    printDateTime(); // Print RTC's date and time
    alarmFlag = false;

    wdt.restart(); // "Pet" the dog
    watchdogInterrupt = 0; // Reset watchdog interrupt counter
  }

  // Check for watchdog interrupt
  if (watchdogFlag)
  {
    Serial.print("Watchdog interrupt: ");
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
  Serial.println();
}

// Power down gracefully
void goToSleep()
{
  // Disable UART
  Serial.end();

  // Disable ADC
  powerControlADC(false);

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

  // Disable all pads (except UART TX/RX)
  for (int x = 0 ; x < 50 ; x++)
    am_hal_gpio_pinconfig(x, g_AM_HAL_GPIO_DISABLE);

  //Power down CACHE, flashand SRAM
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL); // Turn off CACHE and flash
  am_hal_pwrctrl_memory_deepsleep_retain(AM_HAL_PWRCTRL_MEM_SRAM_384K); // Retain all SRAM (0.6 uA)

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
  // Go back to using the main clock
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

  // Renable UART0 pins
  am_hal_gpio_pinconfig(48, g_AM_BSP_GPIO_COM_UART_TX);
  am_hal_gpio_pinconfig(49, g_AM_BSP_GPIO_COM_UART_RX);

  // Renable power to UART0
  am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_UART0);

  // Enable ADC
  initializeADC();

  // Enable ADC
  initializeADC();

  // Enable Serial
  Serial.begin(115200);
}

// Interrupt handler for the RTC
extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt
  rtc.clearInterrupt();

  alarmFlag = true; // Set alarm flag
}

// Interrupt handler for the watchdog.
extern "C" void am_watchdog_isr(void)
{
  // Clear the watchdog interrupt
  wdt.clear();

  // Perform system reset after 10 watchdog interrupts (should not occur)
  if ( watchdogInterrupt < 10 )
  {
    wdt.restart(); // "Pet" the dog
  }
  else {
    while (1); // Wait for reset to occur
  }

  watchdogFlag = true; // Set the watchdog flag
  watchdogInterrupt++; // Increment watchdog interrupt counter
}
