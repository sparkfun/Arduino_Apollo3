/* Auzhor: Owen Lyke
/ Created: May 13 2019
 License: MIT. See SparkFun Arduino Apollo3 Project for more information

 This example demonstrates how to use millis() and micros()
*/

void setup() {

  Serial.begin(9600);

  while(!Serial){}; //Wait for user to open terminal window

  Serial.println("SparkFun Arduino Apollo3 STimer Example");
  Serial.printf("Compiled on %s, %s\n\n", __DATE__, __TIME__);
}

void loop()
{
  Serial.printf("Sec: %d, millis: %d, micros: %d, systicks: 0x%08X, sysoverflows: 0x%08X\n", seconds(), millis(), micros(), systicks(), sysoverflows());
  delay(1111); //Arbitrary delay
}
