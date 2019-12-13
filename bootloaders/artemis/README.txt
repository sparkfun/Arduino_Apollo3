The artemis_svl bootloader is the default bootloader on Artemis module--based boards. A copy of the executable is located here to enable Arduino to re-flash it when needed. To view the source code visit the [SparkFun AmbiqSuite BSPs Repo](https://github.com/sparkfun/SparkFun_Apollo3_AmbiqSuite_BSPs)/common/examples/[artemis_svl](https://github.com/sparkfun/SparkFun_Apollo3_AmbiqSuite_BSPs/tree/master/common/examples/artemis_svl)
The easiest way to upload the SVL is by using the Arduino "burn bootloader" tool and selecting "Ambiq Secure Bootloader" as the "Programmer" option.

Artemis SVL Versions:
v4 - Added peripheral deinitialization to deliver the Apollo3 closer to the initial reset state.
v3 - Initial version