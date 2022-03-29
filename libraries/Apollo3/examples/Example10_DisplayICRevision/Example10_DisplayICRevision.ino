/* Author: Nathan Seidle
  Created: October 16th, 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to display the revision of the Apollo3.
  See the Ambiq website for errata on each revision: https://ambiqmicro.com/mcu/
*/
void setup()
{
    Serial.begin(115200);
    delay(10); //Wait for any bootloader UART interactions to complete
    Serial.println();
    Serial.print("Apollo3 IC revision code: ");

    if (APOLLO3_A0)
    {
        Serial.print("A0");
    }
    else if (APOLLO3_A1)
    {
        Serial.print("A1");
    }
    else if (APOLLO3_B0)
    {
        Serial.print("B0");
    }
    else if (APOLLO3_GE_B0)
    {
        Serial.print("Unknown revision but it's greater than B0");
    }
    else
    {
        Serial.print("Unknown revision");
    }
    Serial.println();
    
    //Unique ChipID
    am_hal_mcuctrl_device_t sDevice;
    am_hal_mcuctrl_info_get(AM_HAL_MCUCTRL_INFO_DEVICEID, &sDevice);

    Serial.print("ChipID0 : 0x");
    Serial.println(sDevice.ui32ChipID0, HEX);
    
    Serial.print("ChipID1 : 0x");
    Serial.println(sDevice.ui32ChipID1, HEX);
    
    Serial.println();
    Serial.println("All done");
}

void loop()
{
    //Do nothing
}