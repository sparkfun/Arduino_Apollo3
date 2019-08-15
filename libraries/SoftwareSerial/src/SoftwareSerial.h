/*
  This is a library written for the SparkFun Artemis

  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!
  https://www.sparkfun.com/artemis
  
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

#ifndef _SoftwareSerial_H
#define _SoftwareSerial_H
#include "Arduino.h"

#define AP3_SS_BUFFER_SIZE 128 //Limit to 8 bits

#define TIMER_FREQ 3000000L

class SoftwareSerial : public Print
{
public:
  SoftwareSerial(uint8_t rxPin, uint8_t txPin, bool invertLogic = false);
  ~SoftwareSerial();

  void listen();
  void stopListening();
  bool isListening();

  void begin(uint32_t baudRate);
  void begin(uint32_t baudRate, HardwareSerial_Config_e SSconfig);
  void end(void);

  int available();
  int read();
  int peek();
  void flush();
  bool overflow();

  virtual size_t write(uint8_t toSend);
  virtual size_t write(const uint8_t *buffer, size_t size);
  virtual size_t write(const char *str);

  void txHandler(void);

  void rxBit(void);
  void rxEndOfByte(void);

  volatile bool rxInUse = false;
  volatile bool txInUse = false;

private:
  ap3_err_t softwareserialSetConfig(HardwareSerial_Config_e SSconfig);
  void startRXListening(void);
  void beginTX();

  uint8_t txBuffer[AP3_SS_BUFFER_SIZE];
  uint16_t txBufferHead = 0;
  volatile uint16_t txBufferTail = 0;
  volatile uint8_t outgoingByte = 0;

  volatile uint8_t rxBuffer[AP3_SS_BUFFER_SIZE];
  volatile uint8_t rxBufferHead = 0;
  uint8_t rxBufferTail = 0;
  volatile uint8_t incomingByte = 0;

  uint8_t _rxPin;
  uint8_t _txPin;

  ap3_gpio_pad_t _txPad;
  ap3_gpio_pad_t _rxPad;

  uint8_t _dataBits = 0; //5, 6, 7, or 8
  uint8_t _parity = 0;   //Type of parity (0, 1, 2)
  uint8_t _stopBits = 0;
  uint8_t _parityBits = 0; //Number of parity bits (0 or 1)
  bool _invertLogic;

  //For RX
  uint16_t rxSysTicksPerBit = 0;
  uint32_t rxSysTicksPerByte = 0;
  uint16_t rxSysTicksPartialBit = 0;
  volatile uint8_t numberOfBits[10];
  volatile uint32_t lastBitTime = 0;
  volatile uint8_t bitCounter;
  volatile bool bitType = false;
  bool _rxBufferOverflow = false;

  //For TX
  void calcParityBit();
  uint16_t txSysTicksPerBit = 0;
  uint16_t txSysTicksPerStopBit = 0;
  uint8_t _parityForByte = 0; //Calculated per byte
  bool _txBufferOverflow = false;
};

#endif
