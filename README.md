# Arduino Core for Apollo3 Microcontroller
==========================================

## Contents
===========
* [Development Status]()
* [Installation]()
* [Issue Template]()
* [Acknowledgements]()

## Development Status
=====================
This core has just been born. Right now you will get compilation and access to the Ambiq SDK functions (currently supporting Ambiq SDK Rel 2.1.0). Board Support Package (BSP) definitions can be used with Arduino **variants**. Currently upload must be performed manually.

## Installation
===============

The structure of this repo is such that it will most closely resemble the structure required when it is time to add it to the Arduino boards manager. Until then you'll have to use the manual installation process.

* Using Arduino IDE Boards Manager (not yet supported)
* [Manually Install in Arduino Sketchbook]()
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