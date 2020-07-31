/*
  Author: Adam Garbo
  Created: July 31st, 2020
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates a simple configuration of the Artemis Watchdog Timer.

  The code will configure the watchdog for both interrupt and reset generation,
  and immediately start the watchdog timer.

  The watchdog ISR provided will 'pet' the watchdog four times. On the fifth
  interrupt, the watchdog will not be pet, so the 'reset' action can occur.

  On the sixth timeout event, the WDT will issue a system reset, and the
  program will start over from the beginning.

  This example is based on the Ambiq SDK 2.4.2 watchdog.c example.
*/

#include <WDT.h>

APM3_WDT wdt;

volatile bool watchdogFlag = false; // Watchdog Timer ISR flag
volatile int watchdogCounter = 0; // Watchdog interrupt counter

unsigned long currentMillis = 0,
              previousMillis = 0;

void setup() {

  Serial.begin(115200);

  Serial.println("Artemis Watchdog Timer Example");

  // Configure the watchdog
  wdt.configure(); // Default interrupt: 5 seconds. Default reset: 15 seconds
  wdt.enable(); // Enable the watchdog
}

void loop() {

  // Check for watchdog interrupts
  if (watchdogFlag) {

    // Calculate duration between watchdog interrupts
    currentMillis = millis() - previousMillis;
    previousMillis = millis();

    Serial.print("Interrupt: "); Serial.print(watchdogCounter); 
    Serial.print(" Period: "); Serial.println(currentMillis); Serial.print(" ms"); 
  }

  watchdogFlag = false; // Clear watchdog flag
  delay(10);
}

// Interrupt handler for the watchdog
extern "C" void am_watchdog_isr(void) {

  // Clear the watchdog interrupt
  wdt.clear();

  // Catch the first four watchdog interrupts, but let the fifth through untouched
  if ( watchdogCounter < 4 ) {
    wdt.restart(); // "Pet" the dog
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH); // Visual indication of imminent system reset
  }

  watchdogFlag = true; // Set watchdog flag
  watchdogCounter++; // Increment watchdog interrupt counter
}
