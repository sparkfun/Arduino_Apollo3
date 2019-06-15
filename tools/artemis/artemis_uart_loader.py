#!/usr/bin/env python
# UART wired update host for Corvette Bootloader

# This script was originally written by Ambiq
# Modified April 2nd, 2019 by SparkFun to auto-bootloading
# Compiled to executable using pyInstaller
# pyinstaller --onefile artemis_uart_loader.py

import argparse
import serial
import serial.tools.list_ports as list_ports
import sys
import time

from sys import exit

# Bootloader command constants
BL_COMMAND_ANNOUNCE = 5
BL_COMMAND_AOK = 6
BL_COMMAND_BAD_CRC = 7
BL_COMMAND_NEXT_FRAME = 8
BL_COMMAND_ALL_DONE = 9
BL_COMMAND_COMPUTER_READY = 10

# ******************************************************************************
#
# Main function
#
# ******************************************************************************


def main():

    # Open a serial port, and communicate with Device
    #
    # Max flashing time depends on the amount of SRAM available.
    # For very large images, the flashing happens page by page.
    # However if the image can fit in the free SRAM, it could take a long time
    # for the whole image to be flashed at the end.
    # The largest image which can be stored depends on the max SRAM.
    # Assuming worst case ~100 ms/page of flashing time, and allowing for the
    # image to be close to occupying full SRAM (256K) which is 128 pages.

    # Check to see if the com port is available
    try:
        with serial.Serial(args.port, args.baud, timeout=1) as ser:
            pass
    except:

        # Show a list of com ports and recommend one
        devices = list_ports.comports()

        # First check to see if user has the given port open
        for dev in devices:
            if(dev.device.upper() == args.port.upper()):
                print(dev.device + " is currently open. Please close any other terminal programs that may be using " +
                      dev.device + " and try again.")
                exit()

        # otherwise, give user a list of possible com ports
        print(args.port.upper() +
              " not found but we detected the following serial ports:")
        for dev in devices:
            if 'CH340' in dev.description:
                print(
                    dev.description + ": Likely an Arduino or derivative. Try " + dev.device + ".")
            elif 'FTDI' in dev.description:
                print(
                    dev.description + ": Likely an Arduino or derivative. Try " + dev.device + ".")
            elif 'USB Serial Device' in dev.description:
                print(
                    dev.description + ": Possibly an Arduino or derivative.")
            else:
                print(dev.description)

        exit()

    # Begin talking over com port
    print('Connecting over serial port {}...'.format(args.port), flush=True)

    # Initially we communicate at 9600
    with serial.Serial(args.port, 9600, timeout=0.100) as ser:

        # DTR is driven low when serial port open. DTR has now pulled RST low causing board reset.
        # If we do not set DTR high again, the Ambiq SBL will not activate, but the SparkFun bootloader will.

        verboseprint("Waiting for command from bootloader")

        # Wait for incoming char(5) indicating ability to boot
        i = 0
        response = ''
        while len(response) == 0:
            i = i + 1
            if(i == 10):
                print("No announcement from Artemis bootloader")
                exit()

            response = ser.read()

            if(len(response) > 0):
                if(ord(response) == BL_COMMAND_ANNOUNCE):
                    # Respond with 'AOK'
                    # values = bytearray([6])
                    ser.write(BL_COMMAND_AOK.to_bytes(1, byteorder='big'))

                    verboseprint("Bootload response received")
                    break
                else:
                    verboseprint("Unkown response: " + str(ord(response)))
                    response = ''

        # Send upload baud rate
        baud_in_bytes = args.baud.to_bytes(4, byteorder='big')
        ser.write(baud_in_bytes)

        # Wait for incoming char indicating bootloader version
        i = 0
        response = ''
        while len(response) == 0:
            i = i + 1
            if(i == 10):
                print("No version from Artemis bootloader")
                exit()

            response = ser.read()

        verboseprint("Bootloader version: " + str(ord(response)))

        ser.flush()
        # Wait for all previous bytes to transmit before changing bauds
        time.sleep(0.010)

        # Go to new baud rate
        ser.baudrate = args.baud

        # Read the binary file from the command line.
        with open(args.binfile, mode='rb') as binfile:
            application = binfile.read()
        # Gather the important binary metadata.
        totalLen = len(application)

        verboseprint("Length to send: " + str(totalLen))

        frame_address = 0
        start = 0
        end = start + 512*4

        # Loop until we have sent the entire file
        while 1:

            # Calc CRC for this chunk
            bytes_to_send = end - start
            words_to_send = bytes_to_send / 4
            myCRC32 = 0
            i = 0
            while i < words_to_send:
                partialStart = int(start + (i * (bytes_to_send/words_to_send)))
                partialEnd = int(
                    end - ((words_to_send - 1 - i) * (bytes_to_send/words_to_send)))

                myCRC32 = myCRC32 + \
                    int.from_bytes(
                        application[partialStart:partialEnd], 'little')

                i = i + 1

            myCRC32 = myCRC32 % 4294967296  # Trim any larger than 32-bit values

            # Tell the target we are ready with new data
            ser.write(BL_COMMAND_COMPUTER_READY.to_bytes(1, byteorder='big'))

            # Wait for incoming BL_COMMAND_NEXT_FRAME indicating the sending of next frame
            i = 0
            response = ''
            while len(response) == 0:
                i = i + 1
                if(i == 10):
                    print("No announcement from Artemis bootloader")
                    exit()

                response = ser.read()

            if ord(response) == BL_COMMAND_NEXT_FRAME:
                verboseprint("Sending next frame: " +
                             str(end - start) + " bytes")

                if(start == totalLen):
                    # We're done!
                    print("Upload complete")

                    # Send size of this frame - special command
                    ser.write(BL_COMMAND_ALL_DONE.to_bytes(2, byteorder='big'))

                    exit()

                # Send size of this frame
                bytes_to_send = end - start
                ser.write(bytes_to_send.to_bytes(2, byteorder='big'))

                # Send start address of this frame
                ser.write(start.to_bytes(4, byteorder='big'))

                # Send our CRC
                ser.write(myCRC32.to_bytes(4, byteorder='big'))

                # Send page of data
                ser.write(application[start:end])

                # Move the pointers foward
                start = end
                end = end + 512*4

                if end > totalLen:
                    end = totalLen
            else:
                print("Unknown BL response")
                exit()

    exit()


# ******************************************************************************
#
# Main program flow
#
# ******************************************************************************
if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        description='SparkFun Serial Bootloader for Artemis')

    parser.add_argument('port', help='Serial COMx Port')

    parser.add_argument('-b', dest='baud', default=115200, type=int,
                        help='Baud Rate (default is 115200)')

    parser.add_argument('-f', dest='binfile', default='',
                        help='Binary file to program into the target device')

    parser.add_argument("-v", "--verbose", default=0, help="Enable verbose output",
                        action="store_true")

    if len(sys.argv) < 2:
        print("No port selected. Detected Serial Ports:")
        devices = list_ports.comports()
        for dev in devices:
            print(dev.description)

    args = parser.parse_args()

    # Create print function for verbose output if caller deems it: https://stackoverflow.com/questions/5980042/how-to-implement-the-verbose-or-v-option-into-a-script
    if args.verbose:
        def verboseprint(*args):
            # Print each argument separately so caller doesn't need to
            # stuff everything to be printed into a single string
            for arg in args:
                print(arg, end=''),
            print()
    else:
        verboseprint = lambda *a: None      # do-nothing function

    main()
