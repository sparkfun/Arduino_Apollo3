/* Author: Owen Lyke
 Created: June 19 2019
 License: MIT. See SparkFun Arduino Apollo3 Project for more information

 This example demonstrates usage of Arduino interrupts.
 All interrupt modes are enabled, however HIGH and LOW are software controlled because the Apollo3 does not
 support HIGH or LOW interrupt states in hardware. 

When HIGH or LOW is selected as an interrupt mode the pad value is read during each ISR call and if the value
no longer matches the mode the ISR flag is cleared. Keep in mind that as long as the interrupt condition
remains true only registered ISRs will execute (in order of first attachment for each pin)

The edge-based interrupts will clear the flag automatically.

*/

#define INT_PIN 16
static uint32_t count = 0;
bool interruptsEnabled = false;

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Interrupt testing");

  pinMode(INT_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, RISING);
  //  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, LOW);
  //  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, HIGH);
  //  attachInterrupt(digitalPinToInterrupt(INT_PIN), myISR, CHANGE);

  //  // attaching a different interrupt to the same pin overwrites the existing ISR
  //  attachInterruptArg(digitalPinToInterrupt(INT_PIN), myISRArg, &count, RISING);

  interruptsEnabled = true;
}

void loop()
{
  count++;
  if (count > 5)
  {
    if (interruptsEnabled)
    {
      detachInterrupt(digitalPinToInterrupt(INT_PIN));
      interruptsEnabled = false;
    }
  }
  delay(1000);
}

void myISR(void)
{
  Serial.println("Hi i am an ISR!");
}

void myISRArg(void *arg)
{
  uint32_t *local_count = (uint32_t *)arg;
  Serial.printf("The time is %d seconds\n", *(local_count));
}
