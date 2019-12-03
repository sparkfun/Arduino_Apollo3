Author: @justiceamoh and Nathan Seidle
Created: November 19th, 2019
License: MIT. See SparkFun Arduino Apollo3 Project for more information

This example demonstrates how to read audio data and output it to a WAV file. A sketch reads PDM data and outputs raw serial, and the ConverToWav python script (python versions 2 and 3 are both available) visualizes and coverts the serial data to a WAV file.

Gotchas:

* You will need to modify the python script to match the COM port used on your computer. For Windows based machines find the 
* You may need to use VLC to play the WAV clips. The bitrate is 256kbps which is higher than some audio players can handle.
* Audio samples are generated fast enough that we need to output serial at 500kbps.