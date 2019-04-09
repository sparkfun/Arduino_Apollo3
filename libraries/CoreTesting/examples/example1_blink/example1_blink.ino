// Author: Owen Lyke
// Created: April 9 2019
// License: MIT. See SparkFun Arduino Apollo3 Project for more information

// This example demonstrates usage of:
// pinMode
// delay

void setup() {
  // put your setup code here, to run once:
  am_util_stdio_terminal_clear();

  am_util_stdio_printf("SparkFun Arduino Apollo3 Blink Example\n");
  am_util_stdio_printf("Compiled on %s, %s\n\n", __DATE__, __TIME__);
  am_bsp_uart_string_print("Hello, World!\r\n");  // Sting_print has less overhead than printf (and less risky behavior since no varargs)
  
  pinMode(37, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(37, LOW);
  delay(1000);
  digitalWrite(37, HIGH);
  delay(1000);
}
