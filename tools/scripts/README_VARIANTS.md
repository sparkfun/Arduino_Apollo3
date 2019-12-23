Variant Tools
=============

The 'single source of truth' for variants is the [SparkFun_AmbiqSuite_Apollo3_BSPs](https://github.com/sparkfun/SparkFun_Apollo3_AmbiqSuite_BSPs) repo. This script is used to sync Arduino's sources with the BSP repo by copying the relevant ```.c``` and ```.h``` files. 

* BSP libraries (```libam_bsp.a```) are not copied because Arduino compiles the bsp files with the core
* BSP source (```bsp_pins.src```) are not copied because you are not meant to edit variants in the Arduino environment