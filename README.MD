![Generate Variants Status Badge](https://github.com/sparkfun/Arduino_Apollo3/workflows/Generate%20Variants/badge.svg)

# SparkFun Ambiq Apollo3 Arduino Core
An mbed-os enabled Arduino core for Ambiq Apollo3 based boards

## Contents
- [SparkFun Ambiq Apollo3 Arduino Core](#sparkfun-ambiq-apollo3-arduino-core)
  - [Contents](#contents)
  - [Installation](#installation)
    - [**Arduino Boards Manager** (recommended)](#arduino-boards-manager-recommended)
    - [**Git** (development)](#git-development)
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
    [https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/master/package_sparkfun_apollo3_index.json](https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/master/package_sparkfun_apollo3_index.json) (raw)
  - Open 'Boards Manager' and select 'SparkFun Apollo3 Boards' - install the latest version
    
### **Git** (development)
  - Ensure the proper directory structure exists by following the [Arduino Boards Manager](#arduino-boards-manager) instructions (install latest)
  - Locate and enter the ```Arduino15``` directory
    - ```cd ~/.../Arduino15```
  - Run the ```dev-install.sh``` script
    - ```./packages/SparkFun/hardware/apollo3/*/tools/scripts/dev-install.sh```

## Development Status

- v2.0.0 based on an [mbed-os](https://github.com/ARMmbed/mbed-os) framework

#### Priorities
- support additional capabilities using Apollo3 peripherals

**Main Arduino Features**
  * Serial: ✅
  * GPIO: ✅
  * Analog / Servo Output: ✅
  * Analog Input: ✅
  * Timing / Delays: ✅

**Standard Libraries**
  * Wire
    * Controller: ✅
    * Peripheral: ❌
  * SPI
    * Controller: ✅
    * Peripheral: ❌
  * Software Serial: ✅

**Apollo3 Specialty Peripherals**
  * PDM Microphones / I2S: ✅
  * BLE: ✅
  * Multi-bit SPI: 🤔
  * I2C/SPI Peripheral: 🤔

## More
### [License](./docs/ISSUES.md)
### [Contributing](./docs/CONTRIBUTING.md)
### [Issue Template](./docs/ISSUES.md)
### [Acknowledgements](./docs/ACKNOWLEDGEMENTS.md)
