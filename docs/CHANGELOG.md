Arduino Apollo3 Core Changelog
==============================

This is a record of the major changes between versions of the SparkFun Arduino Apollo3 core. Only major changes will be recorded for convenience. The complete detail of changes is found in the commit history of this repo.

Each log entry will use the version number of the release that contains the changes listed. Newest version at the top of the file (just below this line)

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