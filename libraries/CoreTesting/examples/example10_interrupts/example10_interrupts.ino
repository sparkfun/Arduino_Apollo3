/* Author: Owen Lyke
 Created: April 9 2019
 License: MIT. See SparkFun Arduino Apollo3 Project for more information

 This example demonstrates usage of Arduino interrupts
*/

#include "structs.h"

#define INT_PIN 16
static uint32_t count = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.print("Interrupt testing");

  am_hal_gpio_pinconfig(INT_PIN, g_deepsleep_button0);  
  
  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, RISING);
//  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, FALLING);
//  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, LOW);
//  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, HIGH);

//  // attaching a different interrupt to the same pin overwrites the existing ISR
//  attachInterruptArg(digitalPinToInterrupt(INT_PIN), myISRArg, &count, RISING);
}

void loop() {
  count++;
  if( count > 5 ){
    detachInterrupt(digitalPinToInterrupt(INT_PIN));
  }
  delay(1000);
}

void myISR( void ){
  Serial.println("Hi i am an ISR!");
}

void myISRArg( void* arg ){
  uint32_t* local_count = (uint32_t*)arg;
  Serial.printf("The time is %d seconds\n", *(local_count) );
}
