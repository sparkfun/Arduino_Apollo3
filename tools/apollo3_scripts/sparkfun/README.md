SparkFun Apollo3 Tools
================================================================

These tools were originally created by Ambiq but were modified by SparkFun for less verbose output (optional) and compiled to make it easier to call from Arduino IDE. Started April 2nd, 2019 by SparkFun to auto-bootloading and Arduino IDE porting.

Python scripts are compiled to executable using pyInstaller, for example:

    pyinstaller --onefile artemis_uart_loader.py

### Three Step Program

To load new firmware onto an Apollo3 the exported/compiled binary must be converted to a non-secure OTA blob. That blob then needs to be converted to a WIRE transmittable blob. Finally, the blob is pushed over serial using uart_wired_update.

Here are the original file names:

* create_cust_image_blob now called artemis_bin_to_blob
* create_cust_wireupdate_blob now called artemis_ota_to_wire.exe
* uart_wired_update_sparkfun now called artemis_uart_loader.exe

File's were copy/pasted to change the file permissions. Otherwise, the generated executables required adminstrative elevation under Windows 10. 

The artemis tools have since forked from the original Ambiq tools.