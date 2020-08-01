/*
  Author: Adam Garbo
  Created: August 1st, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to modify the configuration of the Artemis
  Watchdog Timer (WDT).

  The watchdog timer is controlled by a clock divider, interrupt ticks and 
  reset ticks. To achieve desired watchdog timing, a simple calculation can be made:

  period = # ticks / clock divider frequency 

  Examples: 
  128 interrupt ticks / 128 Hz clock = 1 second
  64 interrupt ticks / 16 Hz clock = 4 seconds
  32 interrupt ticks / 1 Hz clock = 32 seconds
  16 interrupt ticks / 1/16 Hz clock = 256 seconds
  
  The following code will configure the watchdog for both interrupt and reset
  generation, and immediately start the watchdog timer.
  The watchdog ISR provided will 'pet' the watchdog four times. On the fifth
  interrupt, the watchdog will not be pet, so the 'reset' action can occur.
  On the sixth timeout event, the WDT will issue a system reset, and the
  program will start over from the beginning.

  This example is based on the Ambiq SDK 2.4.2 watchdog.c example.

  Tested with SparkFun Artemis Redboard.
*/

#include <WDT.h>

APM3_WDT wdt;

volatile bool watchdogFlag = false; // Watchdog Timer ISR flag
volatile int watchdogInterrupt = 0; // Watchdog interrupt counter

unsigned long currentMillis = 0,
              previousMillis = 0;

void setup()
{
  Serial.begin(115200);

  Serial.println("Artemis Watchdog Timer Example");

  // Configure the watchdog
  /*
    Available watchdog timer clock dividers:
    0 = Low Power Mode. This setting disables the watch dog timer
    1 = 128 Hz 
    2 = 16 Hz 
    3 = 1 Hz
    4 = 1/16th Hz
  */
  // Set watchdog timer clock to 128 Hz
  // Set watchdog interrupt to 1 seconds (128 ticks / 128 Hz = 1 second)
  // Set watchdog reset ~2 seconds (255 ticks / 128 Hz = 1.99 seconds)
  wdt.configure(1, 128, 255); // Note: Ticks are limited to 255 (8-bit)
  wdt.start(); // Start the watchdog
}

void loop()
{
  // Check for watchdog interrupts
  if (watchdogFlag)
  {
    // Calculate duration between watchdog interrupts
    currentMillis = millis() - previousMillis;
    previousMillis = millis();

    Serial.print("Interrupt: "); Serial.print(watchdogInterrupt);
    Serial.print(" Period: "); Serial.print(currentMillis); Serial.println(" ms");

    // The watchdog configurations can also be set individually
    wdt.setClock(2); // Set watchdog timer clock to 16 Hz
    wdt.setInterrupt(64); // Set watchdog interrupt to 4 second (64 ticks / 16 Hz = 4 seconds)
    wdt.setReset(96); // Set watchdog reset to 8 seconds (96 ticks / 16 Hz = 8 seconds)
  }
  watchdogFlag = false; // Clear watchdog flag
  delay(1);
}

// Interrupt handler for the watchdog
extern "C" void am_watchdog_isr(void)
{
  // Clear the watchdog interrupt
  wdt.clear();

  // Catch the first eight watchdog interrupts, but let the ninth through untouched
  if ( watchdogInterrupt < 8 )
  {
    wdt.restart(); // "Pet" the dog
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH); // Visual indication of system reset trigger
  }

  watchdogFlag = true; // Set watchdog flag
  watchdogInterrupt++; // Increment watchdog interrupt counter
}
