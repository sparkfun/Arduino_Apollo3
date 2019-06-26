/*
  Increasing core speed to 96MHz
  By: Nathan Seidle
  SparkFun Electronics
  Date: June 24th, 2019
  License: This code is public domain but the prime number function
  comes from Ambiq/John Burkardt and has a GNU LGPL license. Additional
  code based on Ambiq's burst example.

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15376

  This example shows how to increase the core speed from 48MHz to 96MHz.

  Increasing the core speed will increase current consumption. And increase in core
  speed is useful for calculations and minimizing code run time. Burst mode does not increase
  the speed at which GPIO can toggle.  
*/

long startTime;
long endTime;
int count = 0;

//Make true if you want to print the prime numbers
//Printing output will destroy the accuracy of the timing
bool printThePrimes = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Calculating primes with burst mode");

  startTime = millis();
  count = numberOfPrimes(10000); //Find the number of primes in 10,000
  endTime = millis();

  Serial.printf("Total primes found (should be 1229): %d\n", count);
  Serial.printf("Time required at 48MHz: %dms\n", endTime - startTime);

  enableBurstMode(); //Go to 96MHz
  startTime = millis();
  count = numberOfPrimes(10000); //Find the number of primes in 10,000
  endTime = millis();
  disableBurstMode(); //Return to 48MHz

  Serial.printf("Total primes found (should be 1229): %d\n", count);
  Serial.printf("Time required in burst mode: %dms\n", endTime - startTime);
}

void loop() {
  //Do nothing
}

// Returns the number of primes between 1 and N.
// A naive algorithm is used.
// Input, int N, the maximum number to check.
// Output, int PRIME_NUMBER, the number of prime numbers up to N.
int numberOfPrimes(int maxToSearch)
{
  uint32_t ui32Total, ui32Prime;
  int32_t ix, jx;

  ui32Total = 0;

  for ( ix = 2; ix <= maxToSearch; ix++ )
  {
    ui32Prime = 1;
    for ( jx = 2; jx < ix; jx++ )
    {
      if ( (ix % jx) == 0 )
      {
        ui32Prime = 0;
        break;
      }
    }
    if (ui32Prime == 1 && printThePrimes == true)
    {
      Serial.printf("Prime found: %d\n", ix);
    }
    ui32Total += ui32Prime;
  }
  return ui32Total;
}
