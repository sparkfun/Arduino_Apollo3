Arduino Apollo3 Core Changelog
==============================

This is a record of the major changes between versions of the SparkFun Arduino Apollo3 core. Only major changes will be recorded for convenience. The complete detail of changes is found in the commit history of this repo.

Each log entry will use the version number of the release that contains the changes listed. Newest version at the top of the file (just below this line)

1.0.23 - In Progress
====================

1.0.22 - Dec 13 2019
====================
- Merge #100: SVL Loader to v4 - Adds de-initialization of peripherals. Addresses [this issue](https://forum.sparkfun.com/viewtopic.php?f=168&t=51391&p=210375#p210375)

1.0.21 - Dec 3 2019
===================
- Merge #89: Improvements to SPI library (overhead reduction, configuration checking)
- Unify variants with [SparkFun AmbiqSuite BSPs](https://github.com/sparkfun/SparkFun_Apollo3_AmbiqSuite_BSPs) and make "am_bsp.h" available
- Merge #93 Fixes RTC implementation (correct day of week calculation)
- Merge #95 Makes UART operations non-blocking
- Merge #91 Fixes bitrate in PDM and adds an interactive example for audio playback

1.0.20 - Nov 15 2019
====================
- updated bootloader executables

1.0.18 - Nov 15 2019
====================
- merge #84
- merge #88 Tensorflow Lite Micro library compatibility (changes how ADC is set up)

1.0.17 - Oct 30 2019
===================
- merge #75 EEPROM Improvements
- merge #76 IC Revision Example
- merge #77 Low Power example
- merge #81 Clean SVL
- merge #79 Pullup Resistor defaults
- adds default access to BSP files for all variants

1.0.16 - Oct 14 2019
====================
- merge #73
- merge #72
- revert to artemis_svl.exe from v1.0.12 as stop-gap fix for #74

1.0.15 - Oct 10 2019
====================
- merge PR #70 to allow Arduino CLI to use svl linker script by default
- re-release to include BLE examples

1.0.14 - Oct 4 2019
===================
- regeneration of svl binary

1.0.13 - Sep 30 2019
====================
- merged PR #61 to improve automated testing
- added standard IPAddress files w/ PR #63
- SoftwareSerial now inherits from Stream w/ PR #64
- increased library compatibility w/ PR #66
- improved error handling/reporting in the SparkFun Variable Loader w/ PR #67

1.0.12 - Sep 26 2019
====================
- fixed redefinition of ADC symbols in redboard_artemis_nano
- added empty pins_arduino.h for compile-compatibility iwht older libraries
- added standard IPAddress library
- changed SoftwareSerial to inherit from Stream instead of Print

1.0.10 - Sep 21 2019
====================
- rename board directories to RedBoard. Fix for variant.h not found error.

1.0.9 - Sep 19 2019
===================
- adds Artemis Thing Plus board definitions
- changes board definitions to 'Red' for LTS
- once again based on the temporary core-ble branch that contains a BLE example (mistakenly omitted from v1.0.8)

1.0.8 - Sep 12 2019
===================
- Wire library requestFrom address bug fix (previously required address to be set with 'beginTransmission')
- Correct variant definition for Artemis module (Serial mis-definition)
- Correct variant definition for Edge (Wire1 redefinition)

1.0.7 - Aug 27 2019
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
