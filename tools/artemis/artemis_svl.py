#!/usr/bin/env python
# UART wired update host for Corvette Bootloader

# This script was originally written by Ambiq
# Modified April 2nd, 2019 by SparkFun to auto-bootloading
# Compiled to executable using pyInstaller
# pyinstaller --onefile artemis_svl.py

import argparse
import serial
import serial.tools.list_ports as list_ports
import sys
import time
import re

from sys import exit

# ******************************************************************************
#
# Define Commands
#
# ******************************************************************************
BL_COMMAND_ANNOUNCE = 127
BL_COMMAND_AOK = 6
BL_COMMAND_BAD_CRC = 7
BL_COMMAND_NEXT_FRAME = 8
BL_COMMAND_ALL_DONE = 9
BL_COMMAND_COMPUTER_READY = 10
BL_COMMAND_SET_BAUD = 0x01

# ******************************************************************************
#
# Return byte arrays for given commands
#
# ******************************************************************************
def svl2_cmd_set_baud(baud):
    cmd_bytes = BL_COMMAND_SET_BAUD.to_bytes(1, byteorder = 'big')
    baud_bytes = baud.to_bytes(4, byteorder = 'big')
    return (cmd_bytes + baud_bytes)

def svl2_cmd_host_rdy(start, size, crc32, total_size):
    cmd_bytes = SVL2_CMD_HOST_RDY.to_bytes(1, byteorder = 'big')
    start_bytes = start.to_bytes(4, byteorder = 'big')
    size_bytes = size.to_bytes(4, byteorder = 'big')
    crc32_bytes = crc32.to_bytes(4, byteorder = 'big')
    total_size_bytes = total_size.to_bytes(4, byteorder = 'big')
    return (cmd_bytes + start_bytes + size_bytes + crc32_bytes + total_size_bytes)

# ******************************************************************************
#
# Send commands in the specified format
#
# ******************************************************************************
def send_package(byter, ser):
    num = len(byter)
    num_bytes = num.to_bytes(2, byteorder = 'big')
    ser.write(num_bytes + byter)
    # ser.write()

# ******************************************************************************
#
# Wait for matching command using current port timeout settings
#
# ******************************************************************************
def wait_for_package(byter, ser):
    retval = {'status':-1, 'data':''}                   # Create return dictionary
    data = ser.read_until(byter, len(byter) + 2 + 1)    # Wait for incoming data or timeout (+2 accounts for length bytes, +1 accounts for Apollo3 Serial.begin() blip)  
    reres = re.search(byter,data)                       # Use RE to search for the desired sequence (necessary on Mac/Linux b/c of handling of the startup blip)
    if(reres):
        retval['status'] = 0                # indicate success
        retval['data'] = reres.group(0)     # return the found data

    return retval

# ******************************************************************************
#
# Get a single byte command
#
# ******************************************************************************
def wait_for_command(ser):
    data = ser.read(size = 3)    # Wait for incoming data or timeout    
    data = data[2:]
    return data




# ******************************************************************************
#
# Agree on a baud rate for the target
#
# ******************************************************************************
def phase_set_baud(ser):
    verboseprint("Starting Communications at " + str(args.baud) + " baud")

    baud_set_tries = args.tries
    baud_set_timeout = global_timeout                             # Timeout should be no shorter than (10*len(baud_set_command)/args.baud)
    baud_set_command = svl2_cmd_set_baud(args.baud)

    for tri in range(baud_set_tries):
        send_package(baud_set_command, ser)                                 # Send the baud set command
        baud_set_start = time.time()                                        # Mark the start
        ret = wait_for_package(baud_set_command, ser)                       # Try to get the confirmation
        result = ret['data']
        if(result == baud_set_command):                                     # If there is a match then the target is at the right baud rate
            break
        
        while((time.time() - baud_set_start) < baud_set_timeout):           # Wait for timeout in case target is transmitting slowly
            time.sleep(0.0005)                                              # Mismatch cases:
                                                                            #   Target baud is low, host baud is high:
                                                                            #       Target begins sending, then host reads 'len(baud_set_command)' bytes (incorrectly) and returns. While loop gives target time to finish transmitting
                                                                            #   Target baud is high, host baud is low: 
                                                                            #       Target begins sending, host reads past the end of target transmission, then ready to go again. While loop not needed, but does not hurt
        # ser.write(global_timeout.to_bytes(1,'big'))

    verboseprint('Try #' + str(tri) + '. Result: ' + str(result))
    if(len(result) != len(baud_set_command)):
        print("Baud Set: lengths did not match")
        return 1
    
    if( result != baud_set_command ):
        print("Baud Set: messages did not match")
        return 1

    verboseprint("Received confirmation from target!")
    return 0




# ******************************************************************************
#
# Main function
#
# ******************************************************************************
global_timeout = 0

def main():

    # Open a serial port, and communicate with Device
    
    # Max flashing time depends on the amount of SRAM available.
    # For very large images, the flashing happens page by page.
    # However if the image can fit in the free SRAM, it could take a long time
    # for the whole image to be flashed at the end.
    # The largest image which can be stored depends on the max SRAM.
    # Assuming worst case ~100 ms/page of flashing time, and allowing for the
    # image to be close to occupying full SRAM (256K) which is 128 pages.

    # Begin talking over com port
    print('Connecting over serial port {}...'.format(args.port), flush=True)

    # Set a timeout
    global_timeout = float(args.globaltimeout) # tested as low as 0.03 on some machines - 50 ms works well, 30 ms works sometimes, this delay occurs between the USB-serial converter and when python can see it 
    verboseprint('Using Serial timeout: ' + str(global_timeout))

    # Now open the port for bootloading
    try:
        with serial.Serial(args.port, args.baud, timeout=global_timeout) as ser:

            # DTR is driven low when serial port open. DTR has now pulled RST low causing board reset.
            # If we do not set DTR high again, the Ambiq SBL will not activate, but the SparkFun bootloader will.

            if(phase_set_baud(ser) != 0):
                print('Failed baud set phase')
                exit()

            print('Connected!')
            print('Bootloading...')

            # Wait for incoming BL_COMMAND_ANNOUNCE
            i = 0
            response = ''
            while len(response) == 0:
                i = i + 1
                if(i == 30):
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

            # Read the binary file from the command line.
            with open(args.binfile, mode='rb') as binfile:
                application = binfile.read()
            # Gather the important binary metadata.
            totalLen = len(application)

            verboseprint("Length to send: " + str(totalLen))

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
                        print("Bootloader did not request next frame")
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

    parser.add_argument("-t", "--tries", default=20, help="How many baud rate negotiation messages to send before failing",
                         type=int)

    parser.add_argument("-g", "--globaltimeout", default=0.05, help="Timeout between baud rate negotiation messages, in seconds (default 0.05)")

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