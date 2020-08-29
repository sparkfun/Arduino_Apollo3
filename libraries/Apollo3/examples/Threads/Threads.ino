/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

 /*
 
  RTOS stands for Real Time Operating System, another commonly used term is "scheduler"
  thanks to the fact that the system will schedule different tasks that need to be 
  completed. 

  When one task no longer requires processing time (for example in sleep or delay) the
  RTOS will check to see if any other threads need to be handled.
  https://os.mbed.com/docs/mbed-os/v6.2/apis/scheduling-concepts.html

  An RTOS can help to make groups of logical operations. In this example the main thread
  is used to blink the onboard LED and another thread is used to print status messages 
  over Serial.

*/

#define STATUS_COUNT_TOTAL (10)

rtos::Thread thread;  // create a Thread object (from the rtos namespace)
                      // https://os.mbed.com/docs/mbed-os/v6.2/apis/thread.html

void thread_fn( void ){             // create a function to run as a thread
  Serial.begin(115200);                   // perform setup related to this thread
  Serial.print("Apollo3 - Threads\n\n");

  uint8_t status_count = 0;
  while(status_count < STATUS_COUNT_TOTAL){
    rtos::ThisThread::sleep_for(1000);    // equivalent to calling "delay()"
    Serial.printf("time (ms): %d, status count: %d\n", millis(), status_count++);
  }

  Serial.print("Stopping status thread\n");
  Serial.end();                           // de-initialiaze for this thread
}

// setup() and loop() run within the function main() which is the first thread
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  thread.start(thread_fn);          // assign thread_fn to the object thread and begin execution
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
}
