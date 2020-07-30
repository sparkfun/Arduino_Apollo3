#!/usr/bin/python
from __future__ import division

"""
Author: Justice Amoh
Date: 11/01/2019
Description: Python script to stream audio from Artemis Apollo3 PDM microphone
"""
import sys
import serial
import numpy as np
import matplotlib.pyplot as plt

from serial.tools import list_ports
from time import sleep
from scipy.io import wavfile
from datetime import datetime


# Controls
do_plot  = True
do_save  = True
wavname  = 'recording_%s.wav'%(datetime.now().strftime("%m%d_%H%M"))
runtime  = 50#100                      # runtime in frames, sec/10

# Find Artemis Serial Port
ports = list_ports.comports()
try:
    sPort = [p[0] for p in ports if 'cu.wchusbserial' in p[0]][0]
except Exception as e:
    print 'Cannot find serial port!'
    sys.exit(3)

# Serial Config
ser = serial.Serial(sPort,115200)
ser.reset_input_buffer()
ser.reset_output_buffer()


# Audio Format & Datatype
dtype   = np.int16                  # Data type to read data
typelen = np.dtype(dtype).itemsize  # Length of data type
maxval  = 32768. # 2**15            # For 16bit signed

# Plot Parameters
delay   = .00001                    # Use 1us pauses - as in matlab
fsamp   = 16000                     # Sampling rate
nframes = 10                        # No. of frames to read at a time
buflen  = fsamp//10                 # Buffer length
bufsize = buflen*typelen            # Resulting number of bytes to read
window  = fsamp*10                  # window of signal to plot at a time in samples


# Variables
x = [0]*window
t = np.arange(window)/fsamp         # [x/fsamp for x in range(10)]

#---------------
# Plot & Figures
#---------------
plt.ion()
plt.show()


# Configure Figure
with plt.style.context(('dark_background')):
    fig,axs = plt.subplots(1,1,figsize=(7,2.5))

    lw, = axs.plot(t,x,'r')
    axs.set_xlim(0,window/fsamp)
    axs.grid(which='major', alpha=0.2)
    axs.set_ylim(-1,1)
    axs.set_xlabel('Time (s)')
    axs.set_ylabel('Amplitude')
    axs.set_title('Streaming Audio')
    plt.tight_layout()
    plt.pause(0.001)



# Start Transmission
ser.write('START')          # Send Start command
sleep(1) 

for i in range(runtime):
    buf = ser.read(bufsize)                 # Read audio data
    buf = np.frombuffer(buf,dtype=dtype)    # Convert to int16 
    buf = buf/maxval                        # convert to float
    x.extend(buf)                           # Append to waveform array

    # Update Plot lines
    lw.set_ydata(x[-window:])

    plt.pause(0.001)
    sleep(delay)


# Stop Streaming
ser.write('STOP')
sleep(0.5)
ser.reset_input_buffer()
ser.reset_output_buffer()
ser.close()

# Remove initial zeros
x = x[window:]


# Helper Functions
def plotAll():
    t   = np.arange(len(x))/fsamp
    with plt.style.context(('dark_background')):
        fig,axs = plt.subplots(1,1,figsize=(7,2.5))
        lw,     = axs.plot(t,x,'r')
        axs.grid(which='major', alpha=0.2)
        axs.set_xlim(0,t[-1])
        plt.tight_layout()
    return

# Plot All
if do_plot:
    plt.close(fig)
    plotAll()

# Save Recorded Audio
if do_save:
    wavfile.write(wavname,fsamp,np.array(x))
    print "Recording saved to file: %s"%wavname




