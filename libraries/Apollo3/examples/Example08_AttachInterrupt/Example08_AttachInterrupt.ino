/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#define INT_PIN 16
#define AUTO_PIN D35 // connect this pin to INT_PIN for auto control

rtos::Thread statusThread;

volatile uint32_t count = 0;
uint32_t expected_count = 0;

void status_fn( void ){
  while(1){
    rtos::ThisThread::sleep_for(1000);
    printf("time (ms): %d, count: %d, expected: %d, diff: %d\n", millis(), count, expected_count, (expected_count - count));
    digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN)) ? LOW : HIGH);
  }
}

void myISR(void){
  count++;  // access count because it is a global variable
}

void myParamISR(void *arg){
  uint32_t* pcount = (uint32_t *)arg;
  *(pcount)++;  // access count via the passed in argument
}

void setup()
{
  Serial.begin(115200);
  
  printf("Apollo3 - attachInterrupt\n\n");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(AUTO_PIN, OUTPUT);
  
  digitalWrite(AUTO_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(INT_PIN, INPUT_PULLUP);

  // interrupts can occur on several kinds of conditions
  attachInterrupt(INT_PIN, myISR, RISING);
  attachInterrupt(INT_PIN, myISR, FALLING);
  attachInterrupt(INT_PIN, myISR, LOW);
  attachInterrupt(INT_PIN, myISR, HIGH);
  attachInterrupt(INT_PIN, myISR, CHANGE);

  // there is another function that you can use to attach an interrupt with parameters
  // you can supply a pointer as the 'param' argument, this will be available in the 
  // isr as the void* parameter
  // attachInterruptParam(INT_PIN, myParamISR, INT_MODE, void* param);
  attachInterruptParam(INT_PIN, myParamISR, RISING, (void*)&count);

  // attaching a different interrupt to the same pin overwrites the existing ISR
  attachInterrupt(INT_PIN, myISR, RISING);
  
  // finally you may notice that this example does not use 'digitalPinToInterrupt()'
  // on the Apollo3 all digital pins are interrupts and they are identified by their
  // normal pin numbers
  // you may still use 'digitalPinToInterrupt()' for compatibility with legacy code
  // #define digitalPinToInterrupt(P) (P)
  // attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, INT_MODE);
  // attachInterruptParam(digitalPinToInterrupt(INT_PIN), myParamISR, INT_MODE, void* param);

  statusThread.start(status_fn);
}

void loop()
{
  expected_count++;
  digitalWrite(AUTO_PIN, HIGH);
  digitalWrite(AUTO_PIN, LOW);
}
