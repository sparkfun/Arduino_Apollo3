// Author: Owen Lyke
// Created: May 2 2019
// License: MIT. See SparkFun Arduino Apollo3 Project for more information

// This example demonstrates how to use Arduino HardwareSerial class functions
// Uart objects inherit from HardwareSerial. Declare a Uart object with the following parameters:
// Uart myUart(instance, rx, tx, rts, cts)
//    instance: either 0 or 1, corresponding to uart0 or uart1 in the Apollo3
//    rx pin: (opt.) the *pin* number to use for rx. must map to a valid rx pad on the apollo3 for the uart instance 
//    tx pin: (opt.) the *pin* number to use for tx. must map to a valid tx pad on the apollo3 for the uart instance 
//    rts pin: (opt.) the *pin* number to use for rts. must map to a valid rts pad on the apollo3 for the uart instance 
//    cts pin: (opt.) the *pin* number to use for cts. must map to a valid cts pad on the apollo3 for the uart instance 
// *pin* means the variant's pin number, as opposed to the Apollo3 pad number. The provided number is mapped using the variant's pin map
// Use AP3_UART_PIN_UNUSED if a particular function is unnecessary
// Board-standard Uart objects can be declared in the variant configuration (variant.h and variant.cpp)

// For example, this is the definition of Serial found in the SparkFun Edge variant
//Uart Serial(0, 49, 48);

#define SERIAL_PORT Serial

void setup() {

  SERIAL_PORT.begin(115200);

  //  while(!SERIAL_PORT){}; // todo

  SERIAL_PORT.println("SparkFun Arduino Apollo3 Serial Example");
  SERIAL_PORT.printf("Compiled on %s, %s\n\n", __DATE__, __TIME__); // Look! You can use printf, if you so choose

  SERIAL_PORT.print("Your Apollo3 variant has ");
  SERIAL_PORT.print(AP3_VARIANT_NUM_PINS);
  SERIAL_PORT.println(" pins.");
}

void loop() {

}
