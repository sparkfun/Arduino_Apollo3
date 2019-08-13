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

#ifndef _SoftwareSerial_H
#define _SoftwareSerial_H
#include "Arduino.h"

#define AP3_SS_BUFFER_SIZE 128 //Limit to 8 bits

#define TIMER_FREQ 3000000L

class SoftwareSerial
{
public:
  SoftwareSerial(uint8_t rxPin, uint8_t txPin, bool invertLogic = false);
  ~SoftwareSerial();

  void begin(uint32_t baudRate);
  void begin(uint32_t baudRate, HardwareSerial_Config_e SSconfig);
  void end(void);

  ap3_err_t softwareserialSetConfig(HardwareSerial_Config_e SSconfig);
  int available();
  int read();
  int peek();
  bool overflow();


  void rxBit(void);
  void endOfByte(void);

private:
  void startRXListening(void);

  uint8_t txBuffer[AP3_SS_BUFFER_SIZE];
  uint16_t txBufferHead = 0;
  volatile uint16_t txBufferTail = 0;
  volatile bool txInUse = false;
  volatile uint8_t outgoingByte = 0;

  volatile uint8_t rxBuffer[AP3_SS_BUFFER_SIZE];
  volatile uint8_t rxBufferHead = 0;
  uint8_t rxBufferTail = 0;
  volatile uint8_t incomingByte = 0;

  uint8_t _rxPin;
  uint8_t _txPin;

  uint8_t _indexNumber; // The index number at which the pointer to this instance is stored in the global object table.

  ap3_gpio_pad_t _txPad;
  ap3_gpio_pad_t _rxPad;

  uint8_t _dataBits = 0; //5, 6, 7, or 8
  uint8_t _parity = 0;   //Type of parity (0, 1, 2)
  uint8_t _stopBits = 0;
  uint8_t _parityBits = 0; //Number of parity bits (0 or 1)
  bool _invertLogic;

  //For RX
  uint16_t sysTicksPerBit = 0;
  uint32_t sysTicksPerByte = 0;
  uint16_t sysTicksPartialBit = 0;
  volatile uint8_t numberOfBits[10];
  volatile uint32_t lastBitTime = 0;
  volatile uint8_t bitCounter;
  volatile bool bitType = false;
  volatile bool rxInUse = false;
  bool _rxBufferOverflow = false;

  //For TX
  uint8_t _parityForByte = 0; //Calculated per byte
};

#endif
