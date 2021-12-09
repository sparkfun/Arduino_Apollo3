/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

/*

  The Apollo3 contains two UART peripherals that can be used with
  the Arduino Serial api.
  https://www.arduino.cc/reference/en/language/functions/communication/serial/
  
  It is possible to route each UART instance (0 or 1) to a number of 
  different pads of the Apollo3. Check the datasheet for all possible 
  mappings.
  https://cdn.sparkfun.com/assets/learn_tutorials/9/0/9/Apollo3_Blue_MCU_Data_Sheet_v0_9_1.pdf
  Table 559
  
  You can create new UART objects (which inherit from the HardwareSerial
  class in Arduino) by specifying the TX and RX pins, and optionally the 
  RTS and CTS pins to enable flow control. Here are the constructor protorypes:
  UART(PinName tx, PinName rx, PinName rts, PinName cts);
  UART(pin_size_t tx, pin_size_t rx, pin_size_t rts, pin_size_t cts);
  
  It is possible to use either the mbed style PinName enumerations or the
  Arduino style pin_size_t pin numbers to specify pins. Either case will
  will determine the actuall Apollo3 pad according to the rules explained
  in the DigitalGPIO example.

*/

/*
          Apollo3 UART pad mapping

       uart0         | |       uart1   
---------------------------------------------
 tx | rx | rts | cts | | tx | rx | rts | cts  
---- ---- ----- ----- - ---- ---- ----- -----
  1    2     3     4 | |  8    2    10    11
  7   11     5     6 | | 10    4    16    17
 16   17    13    12 | | 12    9    20    21
 20   21    18    24 | | 14   13    30    26
 22   23    34    29 | | 18   15    31    29
 26   27    35    33 | | 20   19    34    32
 28   29    37    36 | | 24   21    41    36
 30   31    41    38 | | 35   25    44    45
 39   34             | | 37   36            
 41   40             | | 39   38            
 44   45             | | 42   40            
 48   49             | | 46   43            
                     | |      47            
*/

#define BAUD 115200       // any number, common choices: 9600, 115200, 230400, 921600
#define CONFIG SERIAL_8N1 // a config value from HardwareSerial.h (defaults to SERIAL_8N1)

void setup() {
  Serial.begin(BAUD); // set the baud rate with the begin() method
  Serial.println("\n\nApollo3 - Serial");

  // the Apollo3 core supports printf on Serial
  for (size_t idx = 0; idx < 10; idx++){
    Serial.printf("printf supports printing formatted strings! count: %d\n", idx);
  }

  Serial.println("\nEcho... (type characters into the Serial Monitor to see them echo back)\n");
}

void loop() {
  while(Serial.available()){
    Serial.write(Serial.read());
  }
}
