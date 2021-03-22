/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

/*

  SPI functionality is accessed via SPIClass objects or their children.
  On the Apollo3 the MbedSPI class inherits from the SPIClass class and 
  provides the actual implemenation based on mbed-os SPI
  https://os.mbed.com/docs/mbed-os/v6.2/apis/spi.html

  Apollo3 features 6 IOM peripherals that can each be configured as either
  SPI or I2C. Each IOM has only one set of pins that can be used. Check the 
  board schematic or Apollo3 Blue datasheet to determine which IOM
  peripherals are available on your board. There are a few different ways 
  to access these ports:
  - via pre-defined MbedSPI objects like SPI, SPI1 -> SPI5
  - declaring your own MbedSPI object using pins that correspond to the correct IOM

  Once you have an MbedSPI object to work with you can use all the standard 
  Arduino SPI API methods on it
  https://www.arduino.cc/en/reference/SPI

  This example will use threads to organize SPI operations based on board
  capabilities. The test function will check that outgoing bytes are proerly
  detected on a loop-back wire. 

  Wiring:
  For each applicable SPI port:
  SDO <--> SDI
  CLK (Not Connected)

*/

#include "SPI.h"

// define SPI settings at which to test
#define SPI_FREQ 10000000
#define SPI_MODE SPI_MODE3
#define SPI_ORDER MSBFIRST
#define CS_PIN 0

#define SERIAL_PORT Serial

// define pins to create a custom SPI port
// using mbed PinNames or Arduino pin numbers
// (must be all pins from one IOM module)
//#define mySDI D25
//#define mySDO D28
//#define myCLK D27
#if (defined mySDI) && (defined mySDO) && (defined myCLK)
MbedSPI mySPI(mySDI, mySDO, myCLK); // declare the custom MbedSPI object mySPI
#endif

// define a macro to aid testing
#define TEST_SPI_PORT(P)  SERIAL_PORT.printf("testing %s\n\ttime (ms): %d\n\tbyte transer: %s\n\tbuffer transfer: %s\n\n", #P, millis(), ((test_byte_transfer(P) == 0) ? "pass" : "fail"), ((test_buffer_transfer(P) == 0) ? "pass" : "fail"))

// this thread will test the pre-defined SPI object if it exists
rtos::Thread spi_thread;
void spi_thread_fn( void ){
#if VARIANT_SPI_INTFCS > 0
  delay(100);
  SPI.begin();
  while(1){
    TEST_SPI_PORT(SPI);
    delay(500);
  }
#endif
}


// this thread tests the custom mySPI object
#if (defined mySDI) && (defined mySDO) && (defined myCLK)
extern "C" SPIName spi_get_peripheral_name(PinName mosi, PinName miso, PinName sclk); // this mbed internal function determines the IOM module number for a set of pins
rtos::Thread myspi_thread;
void myspi_thread_fn( void ){
  delay(300);
  SERIAL_PORT.printf("starting mySPI on IOM %d\n", spi_get_peripheral_name(mySDO, mySDI, myCLK));
  mySPI.begin();
  while(1){
    TEST_SPI_PORT(mySPI);
    delay(500);
  }
}
#endif

int test_byte_transfer( SPIClass &spi  ){
  uint8_t tx = random(1, 256);
  uint8_t rx = 0x00;
  spi.beginTransaction(SPISettings(SPI_FREQ, SPI_ORDER, SPI_MODE));
  rx = spi.transfer(tx);
  spi.endTransaction();
  if(rx != tx){
    return -1;
  }
  return 0;
}

int test_buffer_transfer( SPIClass &spi ){
  const size_t len = 10;
  uint8_t tx_buff[len];
  uint8_t mirror[len];
  for(size_t idx = 0; idx < len; idx++){
    tx_buff[idx] = random(1, 256);
    mirror[idx] = tx_buff[idx];
  }
  spi.beginTransaction(SPISettings(SPI_FREQ, SPI_ORDER, SPI_MODE));
  spi.transfer(tx_buff, len);
  spi.endTransaction();
  bool mismatched = false;
  for(size_t idx = 0; idx < len; idx++){
    if(tx_buff[idx] != mirror[idx]){
      mismatched = true;
    }
  }
  if(mismatched){
    return -1;
  }
  return 0;
}

void setup() {
  SERIAL_PORT.begin(115200);
  SERIAL_PORT.println("Apollo3 - Simple SPI");
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  spi_thread.start(spi_thread_fn);
  #if (defined mySDI) && (defined mySDO) && (defined myCLK)
  myspi_thread.start(myspi_thread_fn);  
  #endif
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
