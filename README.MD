![Generate Variants Status Badge](https://github.com/sparkfun/Arduino_Apollo3/workflows/Generate%20Variants/badge.svg)

# SparkFun Ambiq Apollo3 Arduino Core
An mbed-os enabled Arduino core for Ambiq Apollo3 based boards

## Contents
- [SparkFun Ambiq Apollo3 Arduino Core](#sparkfun-ambiq-apollo3-arduino-core)
  - [Contents](#contents)
  - [Installation](#installation)
    - [**Arduino Boards Manager** (recommended)](#arduino-boards-manager-recommended)
    - [**Git** (development)](#git-development)
  - [Getting Started](#getting-started)
  - [Development Status](#development-status)
      - [Priorities](#priorities)
  - [More](#more)
    - [License](#license)
    - [Contributing](#contributing)
    - [Issue Template](#issue-template)
    - [Acknowledgements](#acknowledgements)

## Installation
### **Arduino Boards Manager** (recommended)
  - Copy / paste the JSON package URL into the 'Additional Boards Manager URLs' field in Arduino preferences
    ```https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/master/package_sparkfun_apollo3_index.json```
    [https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/master/package_sparkfun_apollo3_index.json](https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/master/package_sparkfun_apollo3_index.json)
  - Open 'Boards Manager' and select 'SparkFun Apollo3 Boards' - install the latest version
    
### **Git** (development)
  - Ensure the proper directory structure exists by following the [Arduino Boards Manager](#arduino-boards-manager) instructions (install latest)
  - navigate to your directory with the latest version
  - rename the directory from 2.x.x to .bckup2.x.x (or simply delete it and download it later if wanted)
  - clone this repo
  `git clone https://github.com/sparkfun/Arduino_Apollo3.git`
  - checkout desired branch
  `cd Arduino_Apollo3`
  `git checkout release-candidate`
  - update all submodules
  `git submodule update --init --recursive`
  - rename directory from Arduino_Apollo3 to 2.x.x from above
  - restart Arduino and enjoy.

## Getting Started
For in depth tutorials, checkout out the [Artemis page](https://www.sparkfun.com/artemis) for the latest. Tutorials that may be of interest include:
 - [Installing Board Definitions in the Arduino IDE](https://learn.sparkfun.com/tutorials/installing-board-definitions-in-the-arduino-ide)
 - [Artemis Development with the Arduino IDE](https://learn.sparkfun.com/tutorials/artemis-development-with-the-arduino-ide)

If you are comfortable with the arduino IDE and already have the boards installed, we recommend trying some examples from within the arduino IDE. Start with the "Built-in Examples" from arduino, but don't forget to checkout out some of the board specific examples that we have included with the package. These demonstrate how to use some of the libraries that make our Artemis board unique, and can take your next project to a new level!

## Development Status

* v2.0.0 Based on an [mbed-os](https://github.com/ARMmbed/mbed-os) framework
* v2.1.0 Contains latest fixes for the v2 core and feature that have been added since v2 release

For more information on how to contribute or how to navigate this repo, check out the [Contributing](./docs/CONTRIBUTING.md) page.

As mentioned MbedOS is the framework to our latest Arduino Core. To contribute to, or use these targets on, MbedOS check out the [main repo by ARM](https://github.com/ARMmbed/mbed-os). This library uses mbed libraries built from a fork of mbed that is maintained by us. The latest version of that repo can be found [here](https://github.com/sparkfun/mbed-os-ambiq-apollo3/tree/ambiq-apollo3-arduino).

**Main Arduino Features**
  * Serial: ✅
  * GPIO: ✅
  * Analog / Servo Output: ✅
  * Analog Input: ✅
  * Timing / Delays: ✅


**Standard Libraries**
  * Wire
    * Controller: ✅
    * Peripheral: 🤔
  * SPI
    * Controller: ✅
    * Peripheral: 🤔
  * EEPROM: ✅

**Apollo3 Specialty Peripherals**
  * PDM Microphones / I2S: ✅
  * BLE: ✅
  * RTC: ✅
  * Watchdog Timer: ✅
  * Burst Mode: ✅

## More
### [License](./docs/LICENSE.md)
### [Contributing](./docs/CONTRIBUTING.md)
### [Issue Template](./docs/ISSUES.md)
### [Acknowledgements](./docs/ACKNOWLEDGEMENTS.md)
