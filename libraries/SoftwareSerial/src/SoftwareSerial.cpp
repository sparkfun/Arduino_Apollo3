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
  Limitations (similar to Arduino core SoftwareSerial):
    * RX on one pin at a time.
    * No TX and RX at the same time.
    * TX gets priority. So if Artemis is receiving a string of characters
    and you do a Serial.print() the print will begin immediately and any additional
    RX characters will be lost. 
    * Uses Timer/Compare module H (aka 7). This will remove PWM capabilities
    on some pins.
    * Parity is supported during TX but not checked during RX.
    * Enabling multiple ports causes 115200 RX to fail (because there is additional instance switching overhead)

    
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

SoftwareSerial *ap3_active_softwareserial_handle = 0;

//Uncomment to enable debug pulses and Serial.prints
//#define DEBUG

#ifdef DEBUG
#define SS_DEBUG_PIN 9
ap3_gpio_pad_t debugPad = ap3_gpio_pin2pad(SS_DEBUG_PIN);
#endif

inline void _software_serial_isr(void *arg)
{
  SoftwareSerial *handle = (SoftwareSerial *)arg;
  handle->rxBit();
}

//Constructor
SoftwareSerial::SoftwareSerial(uint8_t rxPin, uint8_t txPin, bool invertLogic)
{
  _rxPin = rxPin;
  _txPin = txPin;

  _txPad = ap3_gpio_pin2pad(_txPin);
  _rxPad = ap3_gpio_pin2pad(_rxPin);

  _invertLogic = invertLogic;
}

// Destructor
SoftwareSerial::~SoftwareSerial()
{
  end(); // End detaches the interrupt which removes the reference to this object that is stored in the GPIO core
}

void SoftwareSerial::begin(uint32_t baudRate)
{
  begin(baudRate, SERIAL_8N1);
}

void SoftwareSerial::listen()
{
  // Disable the timer interrupt in the NVIC.
  NVIC_DisableIRQ(STIMER_CMPR7_IRQn);

  if (ap3_active_softwareserial_handle != NULL)
    ap3_active_softwareserial_handle->stopListening(); //Gracefully shut down previous instance

  //Point to new instance's cmpr7 ISR
  ap3_active_softwareserial_handle = this;

  lastBitTime = 0; //Reset for next byte

  //Clear pin change interrupt
  am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(_rxPad));

  //Clear compare interrupt
  am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREH);

  //Attach this instance RX pin to PCI
  attachInterruptArg(digitalPinToInterrupt(_rxPin), _software_serial_isr, (void *)this, CHANGE);
}

void SoftwareSerial::stopListening()
{
  // Disable the timer interrupt in the NVIC.
  NVIC_DisableIRQ(STIMER_CMPR7_IRQn);

  detachInterrupt(_rxPin);

  ap3_active_softwareserial_handle == NULL;
}

bool SoftwareSerial::isListening()
{
  return (this == ap3_active_softwareserial_handle);
}

void SoftwareSerial::begin(uint32_t baudRate, HardwareSerial_Config_e SSconfig)
{
  digitalWrite(_txPin, _invertLogic ? LOW : HIGH);
  pinMode(_txPin, OUTPUT);

  pinMode(_rxPin, INPUT);
  if (_invertLogic == false)
    pinMode(_rxPin, INPUT_PULLUP); //Enable external pullup if using normal logic

#ifdef DEBUG
  am_hal_gpio_output_clear(debugPad);
  pinMode(SS_DEBUG_PIN, OUTPUT);
#endif

  // Enable C/T H=7
  am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREH);

  // Don't change from 3MHz system timer, but enable H timer
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ | AM_HAL_STIMER_CFG_COMPARE_H_ENABLE);

  //Set variables data bits, stop bits and parity based on config
  softwareserialSetConfig(SSconfig);

  rxSysTicksPerBit = (TIMER_FREQ / baudRate) * 0.98; //Shorten the number of sysTicks a small amount because we are doing a mod operation
  txSysTicksPerBit = (TIMER_FREQ / baudRate) - 6;    //Shorten the txSysTicksPerBit by the number of ticks needed to run the txHandler ISR

  rxSysTicksPerByte = (TIMER_FREQ / baudRate) * (_dataBits + _parityBits + _stopBits);

  //During RX, if leftover systicks is more than a fraction of a bit, we will call it a bit
  //This is needed during 115200 when cmpr ISR extends into the start bit of the following byte
  rxSysTicksPartialBit = rxSysTicksPerBit / 4;

  txSysTicksPerStopBit = txSysTicksPerBit * _stopBits;

  //Begin PCI
  listen();
}

void SoftwareSerial::end(void)
{
  detachInterrupt(_rxPin);
  // todo: anything else?
}

int SoftwareSerial::available()
{
  return (rxBufferHead + AP3_SS_BUFFER_SIZE - rxBufferTail) % AP3_SS_BUFFER_SIZE;
}

int SoftwareSerial::read()
{
  if (available() == 0)
    return (-1);

  rxBufferTail++;
  rxBufferTail %= AP3_SS_BUFFER_SIZE;
  return (rxBuffer[rxBufferTail]);
}

int SoftwareSerial::peek()
{
  if (available() == 0)
    return (-1);

  uint8_t tempTail = rxBufferTail + 1;
  tempTail %= AP3_SS_BUFFER_SIZE;
  return (rxBuffer[tempTail]);
}

//Wait for TX buffer to get sent
void SoftwareSerial::flush()
{
  while (txInUse)
  {
  }
}

//Returns true if overflow flag is set
//Clears flag when called
bool SoftwareSerial::overflow()
{
  if (_rxBufferOverflow || _txBufferOverflow)
  {
    _rxBufferOverflow = false;
    _txBufferOverflow = false;
    return (true);
  }
  return (false);
}

//Required for print
size_t SoftwareSerial::write(uint8_t toSend)
{
  //As soon as user wants to send something, turn off RX interrupts
  if (txInUse == false)
  {
    detachInterrupt(_rxPin);

    rxInUse = false;
  }

  //See if we are going to overflow buffer
  uint8_t nextSpot = (txBufferHead + 1) % AP3_SS_BUFFER_SIZE;
  if (nextSpot != txBufferTail)
  {
    //Add this byte into the circular buffer
    txBuffer[nextSpot] = toSend;
    txBufferHead = nextSpot;
  }
  else
  {
    _txBufferOverflow = true;
  }

  //See if hardware is available
  if (txInUse == false)
  {
    txInUse = true;

    //Start sending this byte immediately
    txBufferTail = (txBufferTail + 1) % AP3_SS_BUFFER_SIZE;
    outgoingByte = txBuffer[txBufferTail];

    //Calc parity
    calcParityBit();

    beginTX();
  }
  return (1);
}

size_t SoftwareSerial::write(const uint8_t *buffer, size_t size)
{
  for (uint16_t x = 0; x < size; x++)
  {
    write(buffer[x]);
  }
  return (size);
}

size_t SoftwareSerial::write(const char *str)
{
  if (str == NULL)
    return 0;
  return write((const uint8_t *)str, strlen(str));
}

//Starts the transmission of the next available byte from the buffer
void SoftwareSerial::beginTX()
{
  bitCounter = 0;

  //Initiate start bit
  if (_invertLogic == false)
  {
    am_hal_gpio_output_clear(_txPad); //Normal logic, low is start bit
  }
  else
  {
    am_hal_gpio_output_set(_txPad);
  }

  //Clear compare interrupt
  am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREH);

  //Setup ISR to trigger when we are in middle of start bit
  //am_hal_stimer_compare_delta_set(7, txSsysTicksPerBit);
  AM_REGVAL(AM_REG_STIMER_COMPARE(0, 7)) = txSysTicksPerBit; //Direct reg write to decrease execution time

  // Enable the timer interrupt in the NVIC.
  NVIC_EnableIRQ(STIMER_CMPR7_IRQn);
}

//Assumes the global variables have been set: _parity, _dataBits, outgoingByte
//Sets global variable _parityBit
void SoftwareSerial::calcParityBit()
{
  if (_parity == 0)
    return; //No parity

  uint8_t ones = 0;
  for (uint8_t x = 0; x < _dataBits; x++)
  {
    if (outgoingByte & (0x01 << x))
    {
      ones++;
    }
  }

  if (_parity == 1) //Odd
  {
    _parityForByte = !(ones % 2);
  }
  else //Even
  {
    _parityForByte = (ones % 2);
  }
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
  am_hal_gpio_output_set(debugPad);
#endif

  if (lastBitTime == 0)
  {
    bitCounter = 0;
    lastBitTime = bitTime;
    bitType = false;
    rxInUse = true; //Indicate we are now in process of receiving a byte

    //Setup cmpr7 interrupt to handle overall timeout
    //am_hal_stimer_compare_delta_set(7, rxSysTicksPerByte);
    AM_REGVAL(AM_REG_STIMER_COMPARE(0, 7)) = rxSysTicksPerByte; //Direct reg write to decrease execution time

    // Enable the timer interrupt in the NVIC.
    NVIC_EnableIRQ(STIMER_CMPR7_IRQn);
  }
  else
  {
    //Calculate the number of bits that have occured since last PCI
    uint8_t numberOfBits = (bitTime - lastBitTime) / rxSysTicksPerBit;

    if (bitCounter == 0)
    {
      //Catch any partial bits
      //For very high bauds (115200) the final interrupt spills over into the
      //start bit of the next byte. This catches the partial systicks and correctly
      //identifies the start bit as such.
      uint16_t partialBits = (bitTime - lastBitTime) % rxSysTicksPerBit;
      if (partialBits > rxSysTicksPartialBit)
      {
#ifdef DEBUG
//        Serial.println("Partial!");
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
//        Serial.println("Exclude");
#endif
        numberOfBits--; //Exclude parity bit from byte shift
      }
    }

    for (uint8_t y = 0; y < numberOfBits; y++) //Add bits of the current bitType (either 1 or 0) to our byte
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
  am_hal_gpio_output_clear(debugPad);
#endif
}

void SoftwareSerial::rxEndOfByte()
{
  //Finish out bytes that are less than 8 bits
#ifdef DEBUG
//  Serial.printf("bitCounter: %d\n", bitCounter);
//  Serial.printf("incoming: 0x%02X\n", incomingByte);
#endif
  bitCounter--; //Remove start bit from count

  //Edge case where we need to do an additional byte shift because we had data bits followed by a parity bit of same value
  if (_parity)
  {
    bitCounter = bitCounter - _parityBits; //Remove parity bit from count
    if (bitType == true)
      bitCounter++;
  }

#ifdef DEBUG
//  Serial.printf("bitCounter: %d\n", bitCounter);
#endif

  while (bitCounter < 8)
  {
    incomingByte >>= 1;
    if (bitType == true)
      if (bitCounter < _dataBits)
      {
#ifdef DEBUG
//        Serial.println("Add bit");
#endif
        incomingByte |= 0x80;
      }
    bitCounter++;
  }

  //TODO - Check parity bit if parity is enabled

  if (_invertLogic)
    incomingByte = ~incomingByte;

  //See if we are going to overflow buffer
  uint8_t nextSpot = (rxBufferHead + 1) % AP3_SS_BUFFER_SIZE;
  if (nextSpot != rxBufferTail)
  {
    //Add this byte to the buffer
    rxBuffer[nextSpot] = incomingByte;
    rxBufferHead = nextSpot;
  }
  else
  {
#ifdef DEBUG
    am_hal_gpio_output_set(debugPad);
    am_hal_gpio_output_clear(debugPad);
#endif
    _rxBufferOverflow = true;
  }

  lastBitTime = 0; //Reset for next byte

  rxInUse = false; //Release so that we can TX if needed

  // Disable the timer interrupt in the NVIC.
  NVIC_DisableIRQ(STIMER_CMPR7_IRQn);
}

//Called from cmprX ISR
//Sends out a bit with each cmprX ISR trigger
void SoftwareSerial::txHandler()
{
  if (bitCounter < _dataBits) //Data bits 0 to 7
  {
    AM_REGVAL(AM_REG_STIMER_COMPARE(0, 7)) = txSysTicksPerBit; //Direct reg write to decrease execution time
    if (outgoingByte & 0x01)
    {
      am_hal_gpio_output_set(_txPad);
    }
    else
    {
      am_hal_gpio_output_clear(_txPad);
    }
    outgoingByte >>= 1;
    bitCounter++;
  }
  else if (bitCounter == _dataBits) //Send parity bit or stop bit(s)
  {
    if (_parity)
    {
      //Send parity bit
      AM_REGVAL(AM_REG_STIMER_COMPARE(0, 7)) = txSysTicksPerBit; //Direct reg write to decrease execution time
      if (_parityForByte)
      {
        am_hal_gpio_output_set(_txPad);
      }
      else
      {
        am_hal_gpio_output_clear(_txPad);
      }
    }
    else
    {
      //Send stop bit
      AM_REGVAL(AM_REG_STIMER_COMPARE(0, 7)) = txSysTicksPerStopBit; //Direct reg write to decrease execution time
      am_hal_gpio_output_set(_txPad);
    }
    bitCounter++;
  }
  else if (bitCounter == (_dataBits + 1)) //Send stop bit or begin next byte
  {
    if (_parity)
    {
      //Send stop bit
      AM_REGVAL(AM_REG_STIMER_COMPARE(0, 7)) = txSysTicksPerStopBit; //Direct reg write to decrease execution time
      am_hal_gpio_output_set(_txPad);
      bitCounter++;
    }
    else
    {
      //Start next byte
      if (txBufferTail == txBufferHead)
      {
        // Disable the timer interrupt in the NVIC.
        NVIC_DisableIRQ(STIMER_CMPR7_IRQn);

        //All done!
        txInUse = false;

        //Reattach PCI so we can hear rx bits coming in
        listen();
      }
      else
      {
        //Send next byte in buffer
        txBufferTail = (txBufferTail + 1) % AP3_SS_BUFFER_SIZE;
        outgoingByte = txBuffer[txBufferTail];
        calcParityBit();
        beginTX();
      }
    }
  }
  else if (bitCounter == (_dataBits + 2)) //Begin next byte
  {
    //Start next byte
    if (txBufferTail == txBufferHead)
    {
      // Disable the timer interrupt in the NVIC.
      NVIC_DisableIRQ(STIMER_CMPR7_IRQn);

      //All done!
      txInUse = false;

      //Reattach PCI so we can hear rx bits coming in
      listen();
    }
    else
    {
      //Send next byte in buffer
      txBufferTail = (txBufferTail + 1) % AP3_SS_BUFFER_SIZE;
      outgoingByte = txBuffer[txBufferTail];
      calcParityBit();
      beginTX();
    }
  }
}

//Called at the completion of bytes
extern "C" void am_stimer_cmpr7_isr(void)
{
#ifdef DEBUG
  am_hal_gpio_output_set(debugPad);
#endif

  uint32_t ui32Status = am_hal_stimer_int_status_get(false);
  if (ui32Status & AM_HAL_STIMER_INT_COMPAREH)
  {
    am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREH);

    if (ap3_active_softwareserial_handle->rxInUse == true)
    {
      ap3_active_softwareserial_handle->rxEndOfByte();
    }
    else if (ap3_active_softwareserial_handle->txInUse == true)
    {
      ap3_active_softwareserial_handle->txHandler();
    }
  }

#ifdef DEBUG
  am_hal_gpio_output_clear(debugPad);
#endif
}