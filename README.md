# Arduino Core for Apollo3 Microcontroller


## Contents

* [Installation]()
* [Development Status]()
* [Issue Template]()
* [Repo Contents]()
* [Acknowledgements]()


## Installation

The structure of this repo is such that it will most closely resemble the structure required when it is time to add it to the Arduino boards manager. Until then you'll have to use the manual installation process.

* [Using Arduino IDE Boards Manager](https://github.com/sparkfun/Arduino_Boards) (Reccomended)
    * Follow the instructions at the [SparkFun Arduino Boards Repo](https://github.com/sparkfun/Arduino_Boards)
    * Open 'Boards Manager' and select 'SparkFun Apollo3 Boards,' install the latest version.
    * JSON boards manager link for convenience: https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json


* [Manually Install in Arduino Sketchbook]()
    * **First** Install the Arduino SAMD Boards through the Arduino Board Manager. This is required for the ARM Cortex-M toolchain.
    * Open your Arduno sketchbook folder, listed in Arduino's preferences pane (it is where your libraries folder lives)
    * If there is no ```hardware``` folder create it
    * Inside the ```hardware``` folder create ```SparkFun```
    * Choose to clone with Git or download the .zip of this repo
        * Cloning
            * Clone this repo into ```SparkFun``` and give it the name ```apollo3```
        * .ZIP
            * Download the .ZIP of this repository
            * Within ```SparkFun``` create the ```apollo3``` directory
            * Unzip the contents of the .ZIP into the ```apollo3``` directory
    * Restart Arduino IDE for good measure


## Development Status

The basic necesseties are in-place. This means that you can compile and upload code to your Apollo3 board. Development can be done with the whole range of Hardware Abstraction Layer functions provided in the AmbiqSuite Software Development Kit (based on Release2.1.0). 

Current focus is to begin building support for the essential Arduino libraries including:
  * Serial: Fully implemented (will fix bugs and tweak performance over time)
  * GPIO
    * Working:
      * ditigal functions (read / write)
      * analogRead
      * analogWrite
      * interrupts
  * Timing
    * Working:
      * delay
      * delayMicroseconds
      * millis
      * micros
      * seconds (extension)
  * Wire
    * Working: 90% use case, master I2C interface. 
  * SPI
    * Working: transfers with SPISettings.



## How to Contribute
The goal of this Arduino Core is to provide excellent Apollo3 support in a clear and extendable manner. The guidelines for contributing and submitting issues are designed to make it easy to integrate work from many people.  

* **Issue Template**
   * Please do a thorough search of all issues before posting your own. When you're sure that your issue has not been covered please use the [Issue Template](https://github.com/sparkfun/Arduino_Apollo3/blob/master/docs/ISSUE_TEMPLATE.md) to guide your report. Even better, if your issue is a feature request or a fix that you know how to implement please consider contributing with a Pull Request.
   
* **Contributor's Guidelines**
   * Please read and follow the [Contributor's Guidelines](https://github.com/sparkfun/Arduino_Apollo3/blob/master/docs/CONTRIBUTING.md) when making commits for a Pull Request.
   
* **Acknowledgements**
   * The [Acknowledgments](https://github.com/sparkfun/Arduino_Apollo3/blob/master/docs/ACKNOWLEDGEMENTS.md) file exists to easily recognize contributors by name. Detailed contribution information will be recorded by the git commit history. 




## Repo Contents

* cores : source code and headers common to all Apollo3 Arduino boards
* docs : 
    * CONTRIBUTING.md
    * ISSUE_TEMPLATE.md
    * LICENSE.md
* libraries : common source code for optional libraries that pertain to Apollo3 HW
* tools : peripherals such as the serial uploader
* variants : pin mapping and capability listing for individual boards
