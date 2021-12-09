/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

/*

  The Apollo3 microcontroller has "pads" - this is how the processor
  and peripherals expect to be configured. These cannot be changed.

  For convenience you can also use pin names - these are usually 
  created in such a way that they match silkscreen markings on the
  board or explicitly say what the pin is used for.

  Boards in the Apollo3 Arduino core have two kinds of pin names:
  - PinName enum: a text label like "D0" or "LED1"
  - pin_size_t type: a pure number that indicates a pin

  Both kinds can map onto arbitrary pads. Depending on which form 
  you pass into Arduino functions the appropriate map will be used.
  - PinName: uses the PinName enumeration in
      cores/mbed-os/targets/TARGET_Ambiq_Micro/TARGET_Apollo3/${VARIANT}/PinNames.h
  - pin indices: match with pin states in variantPinStates array in:
      variants/${VARIANT}/config/pins.cpp

  It is also possible for Arduino to define some symbols that serve
  as additional names for pins

  In this example we will utilize those methods as well as one more
  to blink the built-in LED on the board

  reference: https://www.arduino.cc/reference/en/language/functions/digital-io/digitalwrite/

*/

// Arduino boards typically define 'LED_BUILTIN'
// This symbol expands to the pin_size_t corresponding to the 
// onboard LED
const pin_size_t ledPinNumber = LED_BUILTIN;

// Mbed boards use a PinName called LED1 to indicate
// the built-in LED
const PinName ledPinName = LED1;

void blinkWithNumber(pin_size_t number){
  digitalWrite(number, HIGH);
  delay(250);
  digitalWrite(number, LOW);
  delay(500);
}

void blinkWithName(PinName name){
  digitalWrite(name, HIGH);
  delay(250);
  digitalWrite(name, LOW);
  delay(500);
}

void setup() {
  // Arduino functions that take pin arguemnts can use either type 
  // Here we are using the PinName type

  pinMode(ledPinName, OUTPUT);
}

void loop() {
  // When digitalWrite is called (with either kind of variable)
  // it searches the variantPinStates array for a match. If one
  // is found then an internal function sets the state of the
  // corresponding pad

  blinkWithNumber(ledPinNumber);
  blinkWithName(ledPinName);
}
