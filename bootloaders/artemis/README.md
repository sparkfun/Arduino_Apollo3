Artemis Bootloader
==========================

The artemis_svl bootloader is the default bootloader on Artemis module--based boards. A copy of the binary is located here to enable Arduino to re-flash it when needed. To view the source code visit the [Apollo3 BSP](https://github.com/sparkfun/SparkFun_Apollo3_AmbiqSuite_BSPs/tree/master/common/examples/artemis_svl).

The bootloader for the Artemis can be upgraded via Arduino using [these instructions](https://learn.sparkfun.com/tutorials/designing-with-the-sparkfun-artemis/all#troubleshooting). It can also be upgraded using the [Artemis Firmware Upload GUI](https://github.com/sparkfun/Artemis-Firmware-Upload-GUI).

Artemis SVL Versions:

* v5 - Speed and reliability increases
* v4 - Added peripheral deinitialization to deliver the Apollo3 closer to the initial reset state.
* v3 - Initial version