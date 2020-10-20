# Variant BSP
The Board Support Package (BSP) is a feature of the AmbiqSuite SDK that allows you to:
- define names and default configurations for pins
- write globally-accessible routines for common functions

These features are potentially useful when:
- designing an Apollo3-based board for a custom purpose where pin functions will be mainly static
- using AmbiqSuite examples within Arduino

For an example of a BSP see the AmbiqSuite release 2.1.0 SDK under 'SDK/boards/Apollo3_EVB/bsp'

To include BSP files (i.e. am_bsp.h, am_bsp.c, am_bsp_pins.h, am_bsp_pins.c, as well as 
bsp_pins.src and pinconfig.py) add " -I{build.variant.path}/bsp" to the board definition in 
boards.txt as part of the board.build.includes parameter.

