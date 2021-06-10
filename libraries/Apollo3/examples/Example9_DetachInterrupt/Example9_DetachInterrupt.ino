/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// Wiring:
// INT_PIN <-> AUTO_PIN (for automatic interrupt generation)

#define INT_PIN 16
#define AUTO_PIN D35 // connect this pin to INT_PIN for auto control
#define INT_TYPE RISING

rtos::Thread autoISRThread;
rtos::Thread heartbeatThread;

volatile uint32_t count = 0;
uint32_t expected_count = 0;
volatile bool isr_flag = false;

void myParamISR( void *param ){
  isr_flag = true;
  volatile uint32_t* pcount = (volatile uint32_t *)param;
  *pcount += 1; // access count via the passed in parameter
}

void generateAutoISRs( void ){
  while(1){
    delay(1000);
    expected_count++;
    digitalWrite(AUTO_PIN, HIGH);
    digitalWrite(AUTO_PIN, LOW);
    digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN)) ? LOW : HIGH);
    if(INT_TYPE == CHANGE){
      expected_count++;
    } 
  }
}

void hearbeat( void ){
  while(1){
    delay(10000);
    printf("blub blub. current time (ms): %d\n", millis());
  }
}

void setup()
{
  Serial.begin(115200);
  printf("Apollo3 - detachInterrupt\n\n");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(AUTO_PIN, OUTPUT);
  digitalWrite(AUTO_PIN, LOW);

  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterruptParam(INT_PIN, myParamISR, INT_TYPE, (void*)&count);

  autoISRThread.start(generateAutoISRs);
  heartbeatThread.start(hearbeat);
}

void loop()
{
  if(isr_flag){
    isr_flag = false;
    printf("isr fired. count: %d, expected: %d, diff: %d, time (ms): %d\n", count, expected_count, (expected_count - count), millis());
    if(count >= 5){
      count = 0;
      printf("shutting off interrupts\n\n");
      detachInterrupt(INT_PIN);
    }
  }
}
