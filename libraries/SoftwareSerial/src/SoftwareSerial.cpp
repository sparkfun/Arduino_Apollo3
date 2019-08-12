/*
  This is a library written for the SparkFun Artemis

  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!
  https://www.sparkfun.com/products/15332
  https://www.sparkfun.com/products/15376
  https://www.sparkfun.com/products/15411
  https://www.sparkfun.com/products/15412

  Written by Nathan Seidle @ SparkFun Electronics, August 12th, 2019

  https://github.com/sparkfun/SparkFun_Apollo3

  SoftwareSerial support for the Artemis
  Any pin can be used for software serial receive or transmit 
  at 300 to 115200bps and anywhere inbetween.
  Limitations:
    Uses Timer/Compare module H. This will remove PWM capabilities
    on some pins.
    Parity is supported but not checked during RX.
    
  Development environment specifics:
  Arduino IDE 1.8.x

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SoftwareSerial.h"
#include "Arduino.h"

// Global Table of SoftwareSerial Pointers
SoftwareSerial* gpSoftwareSerialObjs[AP3_GPIO_MAX_PADS];
uint8_t gSoftwareSerialNumObjs = 0;


// Software Serial ISR (To attach to pin change interrupts)
void _software_serial_isr( void ){
  uint64_t gpio_int_mask = 0x00;
  am_hal_gpio_interrupt_status_get(true, &gpio_int_mask);
  SoftwareSerial* obj = NULL;
  for(uint8_t indi = 0; indi < gSoftwareSerialNumObjs; indi++){
    obj = gpSoftwareSerialObjs[indi];
    if(obj == NULL){
      break; // there should not be any null pointers in the global object table
    }
    if(obj->_rxPadBitMask & gpio_int_mask){
      obj->rxBit();
    }
  }
}


//Constructor
SoftwareSerial::SoftwareSerial(uint8_t rxPin, uint8_t txPin)
{
  if( gSoftwareSerialNumObjs >= AP3_GPIO_MAX_PADS ){
    return; // Error -- no instances left to create
  }

  _rxPin = rxPin;
  _txPin = txPin;

  _txPad = ap3_gpio_pin2pad(_txPin);
  _rxPad = ap3_gpio_pin2pad(_rxPin);

  _rxPadBitMask = ( 0x01 << _rxPad );

  // Add to the global array
  _indexNumber = gSoftwareSerialNumObjs;
  gpSoftwareSerialObjs[_indexNumber] = this;
  gSoftwareSerialNumObjs++;
}

// Destructor
SoftwareSerial::~SoftwareSerial()
{
  if( gSoftwareSerialNumObjs < 1 ){
    return; // error -- no instances left to destroy
  }

  // Remove from global pointer list by filtering others down:
  uint8_t index = _indexNumber;
  do{
    gpSoftwareSerialObjs[index] = NULL;
    if( index < (gSoftwareSerialNumObjs-1) ){
      gpSoftwareSerialObjs[index] = gpSoftwareSerialObjs[index+1];
    }
    index++;
  }while( index < gSoftwareSerialNumObjs );
  gSoftwareSerialNumObjs--;
}

void SoftwareSerial::begin(uint32_t baudRate)
{
  begin(baudRate, SERIAL_8N1);
}

void SoftwareSerial::begin(uint32_t baudRate, HardwareSerial_Config_e SSconfig)
{
  pinMode(_txPin, OUTPUT);
  pinMode(_rxPin, INPUT_PULLUP);

  am_hal_gpio_output_set(_txPad);

  // Enable C/T H=7
  am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREH);

  // Don't change from 3MHz system timer, but enable H timer
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ | AM_HAL_STIMER_CFG_COMPARE_H_ENABLE);

  //Set variables data bits, stop bits and parity based on config
  softwareserialSetConfig(SSconfig);

  sysTicksPerBit = (TIMER_FREQ / baudRate) * 0.98; //Short the number of sysTicks a small amount because we are doing a mod operation

  sysTicksPerByte = (TIMER_FREQ / baudRate) * (_dataBits + _parityBits + _stopBits);

  //During RX, if leftover systicks is more than a fraction of a bit, we will call it a bit
  //This is needed during 115200 when cmpr ISR extends into the start bit of the following byte
  sysTicksPartialBit = sysTicksPerBit / 4;

  //Clear pin change interrupt
  am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(_rxPad));

  //Clear compare interrupt
  am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREH);

  attachInterrupt(digitalPinToInterrupt(_rxPin), _software_serial_isr, CHANGE);
}

ap3_err_t SoftwareSerial::softwareserialSetConfig(HardwareSerial_Config_e SSconfig)
{
  ap3_err_t retval = AP3_OK;
  switch (SSconfig)
  {
  case SERIAL_5N1:
    _dataBits = 5;
    _parity = 0;
    _stopBits = 1;
    break;
  case SERIAL_6N1:
    _dataBits = 6;
    _parity = 0;
    _stopBits = 1;
    break;
  case SERIAL_7N1:
    _dataBits = 7;
    _parity = 0;
    _stopBits = 1;
    break;
  case SERIAL_8N1:
    _dataBits = 8;
    _parity = 0;
    _stopBits = 1;
    break;
  case SERIAL_5N2:
    _dataBits = 5;
    _parity = 0;
    _stopBits = 2;
    break;
  case SERIAL_6N2:
    _dataBits = 6;
    _parity = 0;
    _stopBits = 2;
    break;
  case SERIAL_7N2:
    _dataBits = 7;
    _parity = 0;
    _stopBits = 2;
    break;
  case SERIAL_8N2:
    _dataBits = 8;
    _parity = 0;
    _stopBits = 2;
    break;
  case SERIAL_5E1:
    _dataBits = 5;
    _parity = 2;
    _stopBits = 1;
    break;
  case SERIAL_6E1:
    _dataBits = 6;
    _parity = 2;
    _stopBits = 1;
    break;
  case SERIAL_7E1:
    _dataBits = 7;
    _parity = 2;
    _stopBits = 1;
    break;
  case SERIAL_8E1:
    _dataBits = 8;
    _parity = 2;
    _stopBits = 1;
    break;
  case SERIAL_5E2:
    _dataBits = 5;
    _parity = 2;
    _stopBits = 2;
    break;
  case SERIAL_6E2:
    _dataBits = 6;
    _parity = 2;
    _stopBits = 2;
    break;
  case SERIAL_7E2:
    _dataBits = 7;
    _parity = 2;
    _stopBits = 2;
    break;
  case SERIAL_8E2:
    _dataBits = 8;
    _parity = 2;
    _stopBits = 2;
    break;
  case SERIAL_5O1:
    _dataBits = 5;
    _parity = 1;
    _stopBits = 1;
    break;
  case SERIAL_6O1:
    _dataBits = 6;
    _parity = 1;
    _stopBits = 1;
    break;
  case SERIAL_7O1:
    _dataBits = 7;
    _parity = 1;
    _stopBits = 1;
    break;
  case SERIAL_8O1:
    _dataBits = 8;
    _parity = 1;
    _stopBits = 1;
    break;
  case SERIAL_5O2:
    _dataBits = 5;
    _parity = 1;
    _stopBits = 2;
    break;
  case SERIAL_6O2:
    _dataBits = 6;
    _parity = 1;
    _stopBits = 2;
    break;
  case SERIAL_7O2:
    _dataBits = 7;
    _parity = 1;
    _stopBits = 2;
    break;
  case SERIAL_8O2:
    _dataBits = 8;
    _parity = 1;
    _stopBits = 2;
    break;
  default:
    retval = AP3_INVALID_ARG;
    break;
  }

  _parityBits = 0;
  if (_parity)
    _parityBits = 1;

  return retval;
}

//ISR that is called each bit transition on RX pin
void SoftwareSerial::rxBit(void)
{
  uint32_t bitTime = CTIMER->STTMR; //Capture current system time

#ifdef DEBUG
  am_hal_gpio_output_set(triggerPad);
#endif

  if (lastBitTime == 0)
  {
    bitCounter = 0;
    lastBitTime = bitTime;
    bitType = false;

    //Setup cmpr7 interrupt to handle overall timeout
    //am_hal_stimer_compare_delta_set(7, sysTicksPerByte);
    AM_REGVAL(AM_REG_STIMER_COMPARE(0, 7)) = sysTicksPerByte; //Direct reg write to decrease execution time

    // Enable the timer interrupt in the NVIC.
    NVIC_EnableIRQ(STIMER_CMPR7_IRQn);
  }
  else
  {
    //Calculate the number of bits that have occured since last PCI
    //Then add those bits of the current bitType (either 1 or 0) to
    //the byte
    uint8_t numberOfBits = (bitTime - lastBitTime) / sysTicksPerBit;

    if (bitCounter == 0)
    {
      //Catch any partial bits
      //For very high bauds (115200) the final interrupt spills over into the
      //start bit of the next byte. This catches the partial systicks and correctly
      //identifies the start bit as such.
      uint16_t partialBits = (bitTime - lastBitTime) % sysTicksPerBit;
      if (partialBits > sysTicksPartialBit)
      {
#ifdef DEBUG
        Serial.println("Partial!");
#endif
        numberOfBits++;
      }

      //Exclude start bit from byte shift
      bitCounter++;
      numberOfBits--;
    }

    if (_parity)
    {
      if (numberOfBits + bitCounter > _dataBits + _parityBits)
      {
#ifdef DEBUG
        Serial.println("Exclude");
#endif
        numberOfBits--; //Exclude parity bit from byte shift
      }
    }

    for (uint8_t y = 0; y < numberOfBits; y++) //Number of bits in this chunk of time
    {
      incomingByte >>= 1;
      if (bitType == true)
        incomingByte |= 0x80;
    }
    bitCounter += numberOfBits;
    bitType = !bitType;    //Next bit will be inverse of this bit
    lastBitTime = bitTime; //Remember this bit time as the starting time for the next PCI
  }

#ifdef DEBUG
  am_hal_gpio_output_clear(triggerPad);
#endif
}
