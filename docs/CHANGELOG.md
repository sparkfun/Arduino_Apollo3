Arduino Apollo3 Core Changelog
==============================

This is a record of the major changes between versions of the SparkFun Arduino Apollo3 core. Only major changes will be recorded for convenience. The complete detail of changes is found in the commit history of this repo.

Each log entry will use the version number of the release that contains the changes listed. Newest version at the top of the file (just below this line)

1.0.15
===================
- merge PR #70 to allow Arduino CLI to use svl linker script by default
- re-release to include BLE examples

1.0.14
===================
- regeneration of svl binary

1.0.13
===================
- merged PR #61 to improve automated testing
- added standard IPAddress files w/ PR #63
- SoftwareSerial now inherits from Stream w/ PR #64
- increased library compatibility w/ PR #66
- improved error handling/reporting in the SparkFun Variable Loader w/ PR #67

1.0.12
===================
- fixed redefinition of ADC symbols in redboard_artemis_nano
- added empty pins_arduino.h for compile-compatibility iwht older libraries
- added standard IPAddress library
- changed SoftwareSerial to inherit from Stream instead of Print

1.0.8
===================
- Wire library requestFrom address bug fix (previously required address to be set with 'beginTransmission')
- Correct variant definition for Artemis module (Serial mis-definition)
- Correct variant definition for Edge (Wire1 redefinition)

1.0.7 - 
===================
- Rectified error in Artemis variant pin map (pad 46 was listed as available when it is in fact not)
- Fixed CPU hang on certain 'analogWrite' calls

1.0.6 - Aug 5 2019
==================
- Added default SD support with Arduino SD library
- Fixed non-return errors in SPI library
- Added full duplex capability to 'transfer(uint8_t)'

1.0.5 - Jul 30 2019
===================
- Minor updates to bootloaders
    - SVL: added some exits and improved verbose/quiet formatting
    - ASB (SBL): reduced impact of UART hammering with 'reset_input_buffer'

1.0.4 - Jul 25 2019
===================
- Implement Artemis SVL v3

1.0.3 - Jul 16 2019
===================
- Fix platform discrepancies in upload tools (SVL)

1.0.2 - Jul 10 2019
===================
- Fix upload tools (single file binaries w/ executable permissions)
- Add baud rate selection for Ambiq SBL on Edge variant
- Improve PWM output function 

1.0.1 - Jul 1 2019
==================
- Add burst mode support (PR #13)
- Made examples under 'Core Testing' visible through File->Examples menu (PR #10)
- Add EEPROM library (PR #11)
- Fix handling of spaces in paths (PR #20)
- Fix paths to SparkFun SVL for Mac and Linux
- Add ADC power functions and Serial end/flush (PR #18)

1.0.0 - Jun 20 2019
===================
- Add SparkFun Variable Bootloader
- Better support for malloc + related functions


0.0.0 - Jun 14 2019
===================
Initial release
