/*
  Author: Adam Garbo
  Created: February 1st, 2021
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates a simple configuration of the Artemis Watchdog Timer.

  The code will configure the watchdog for both interrupt and reset generation,
  and immediately start the watchdog timer.

  The watchdog ISR provided will 'pet' the watchdog four times. On the fifth
  interrupt, the watchdog will not be pet, so the 'reset' action can occur.
  On the sixth timeout event, the WDT will issue a system reset, and the
  program will start over from the beginning.

  The watchdogCounter will show the number of ticks before the watchdog
  reset occurs.

  This example is based on the Ambiq SDK 2.4.2 watchdog.c example.
  
  Tested with SparkFun Artemis Redboard.
*/

#include "WDT.h"

volatile bool watchdogFlag = false; // Watchdog Timer ISR flag
volatile int  watchdogInterrupt = 0; // Watchdog interrupt counter
unsigned long currentMillis = 0,
              previousMillis = 0;

void setup() {

  Serial.begin(115200);

  Serial.println("Artemis Watchdog Timer Example");
  
  pinMode(LED_BUILTIN, OUTPUT);

  // Start the watchdog
  wdt.start();
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
    Serial.print(" Period: "); Serial.print(currentMillis); Serial.println(" ms ");

    if (watchdogInterrupt == 5) 
    {
      Serial.println("Warning: Watchdog has triggered a system reset");
    }
  }
  watchdogFlag = false; // Clear watchdog flag
  delay(1);
}

// Interrupt handler for the watchdog
extern "C" void am_watchdog_isr(void)
{
  // Clear the watchdog interrupt
  wdt.clear();

  // Catch the first four watchdog interrupts, but let the fifth through untouched
  if ( watchdogInterrupt < 4 )
  {
    wdt.restart(); // "Pet" the dog
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH); // Visual indication of system reset trigger
  }

  watchdogFlag = true; // Set watchdog flag
  watchdogInterrupt++; // Increment watchdog interrupt counter
}
