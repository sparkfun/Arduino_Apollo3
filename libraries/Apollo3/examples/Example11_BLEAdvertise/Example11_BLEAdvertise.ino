/*
  BLEAdvertise

  This example creates a BLE peripheral which advertises itself as Artemis.
  Central devices may connect with it, but there are no services or 
  characteristics to interact with.

  Based on a stripped down version of the LED example from the ArduinoBLE examples
*/

#include <ArduinoBLE.h> //http://librarymanager/All#ArduinoBLE_IoT

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Artemis");

  // start advertising
  BLE.advertise();

  Serial.println("BLE advertising as 'Artemis'");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral...
    while (central.connected()) {
      //Nothing to do here
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
