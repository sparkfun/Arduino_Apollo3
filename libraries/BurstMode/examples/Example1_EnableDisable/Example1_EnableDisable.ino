//Burst Mode example
//TEST_PIN is toggled as fast as possible, with burst mode enable and disabled.
//Use a logic analyzer to view the difference in output
#include "BurstMode.h"

#define TEST_PIN D2 //Change to whatever pin you would like

#define NUM_TOGGLES 20 //Number of times to toggle the gpio as an example task

void setup() {
  Serial.begin(115200); 
  Serial.println("BurstMode Example1 - Enable and Disable");
}

void loop() {
  long startTime, endTime;
  enableBurstMode(); //Go to 96MHz
  //Toggle the with digital write as fast as possible, 20 times
  startTime = micros();
  for(int i = 0; i < NUM_TOGGLES; i++)
  {
    digitalWrite(TEST_PIN, HIGH);
    digitalWrite(TEST_PIN, LOW);
  }
  endTime = micros();
  Serial.printf("Time (in microseconds) to toggle GPIO %d times = %d, at clock speed %d \r\n",
                                              NUM_TOGGLES, (endTime - startTime), getCpuFreqMHz());

  disableBurstMode(); //Go back to 48MHz
  //Toggle the with digital write as fast as possible, 20 times
  startTime = micros();
  for(int i = 0; i < NUM_TOGGLES; i++)
  {
    digitalWrite(TEST_PIN, HIGH);
    digitalWrite(TEST_PIN, LOW);
  }
  endTime = micros();
  Serial.printf("Time (in microseconds) to toggle GPIO %d times = %d, at clock speed %d \r\n",
                                              NUM_TOGGLES, (endTime - startTime), getCpuFreqMHz());
  
  delay(1000);
}