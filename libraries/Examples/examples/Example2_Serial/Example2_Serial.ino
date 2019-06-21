/* Author: Nathan Seidle
  Created: June 12th, 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates usage of:
  Serial
  Serial1

  This example works best on the BlackBoard Artemis and BlackBoard Artemis ATP that have
  the TX1 and RX1 pins exposed. See the advanced serial example for setting up a serial
  port on other pins and other carrier boards.
*/

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello debug window!");

  Serial1.begin(115200);
  Serial1.println("This prints on TX1/RX1 pins");
}

void loop()
{
  Serial.println("Time for a menu:");
  Serial.println("a) Activate the cheese");
  Serial.println("x) Exit");
  Serial.println();

  while (Serial.available() == true)
    Serial.read(); //Read any characters that may be sitting in the RX buffer before we wait for user input

  while (Serial.available() == false)
    delay(1); //Wait for user input

  char incoming = Serial.read();

  if (incoming == 'a' || incoming == 'A')
  {
    Serial.println("How many cheeses to dispense?");

    while (Serial.available() == true)
      Serial.read(); //Read any characters that may be sitting in the RX buffer before we wait for user input

    while (Serial.available() == false)
      ;                                           //Wait for user to type a number
    unsigned int cheeseCount = Serial.parseInt(); //That's a lot of potential cheese

    //Print to second serial port
    for (unsigned int x = 0; x < cheeseCount; x++)
    {
      Serial1.println("Cheese!");
    }

    Serial.printf("Thank you. %d cheeses dispensed.\n\r", cheeseCount); //Did you see that?! We used printf!
  }
  else if (incoming == 'x' || incoming == 'X')
  {
    Serial.println("Exiting... Have a nice day.");
    while (1)
      ; //Freeze
  }
  else
  {
    Serial.print("Unknown choice: ");
    Serial.write(incoming);
    Serial.println();
  }
}
