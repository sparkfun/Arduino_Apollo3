#!/usr/bin/env python
# SparkFun Variable Loader
# Variable baud rate bootloader for Artemis Apollo3 modules

# Immediately upon reset the Artemis module will search for the timing character
#   to auto-detect the baud rate. If a valid baud rate is found the Artemis will 
#   respond with the bootloader version packet
# If the computer receives a well-formatted version number packet at the desired
#   baud rate it will send a command to begin bootloading. The Artemis shall then 
#   respond with the a command asking for the next frame. 
# The host will then send a frame packet. If the CRC is OK the Artemis will write 
#   that to memory and request the next frame. If the CRC fails the Artemis will
#   discard that data and send a request to re-send the previous frame.
# This cycle repeats until the Artemis receives a done command in place of the
#   requested frame data command.
# The initial baud rate determination must occur within some small timeout. Once 
#   baud rate detection has completed all additional communication will have a 
#   universal timeout value. Once the Artemis has begun requesting data it may no
#   no longer exit the bootloader. If the host detects a timeout at any point it 
#   will stop bootloading. 

# Notes about PySerial timeout:
# The timeout operates on whole functions - that is to say that a call to 
#   ser.read(10) will return after ser.timeout, just as will ser.read(1) (assuming 
#   that the necessary bytes were not found)
# If there are no incoming bytes (on the line or in the buffer) then two calls to 
#   ser.read(n) will time out after 2*ser.timeout
# Incoming UART data is buffered behind the scenes, probably by the OS.

# ***********************************************************************************
#
# Imports
#
# ***********************************************************************************

import argparse
import serial
import serial.tools.list_ports as list_ports
import sys
import time
import math
from sys import exit

# ***********************************************************************************
#
# Commands
#
# ***********************************************************************************
SVL_CMD_VER     = 0x01  # version
SVL_CMD_BL      = 0x02  # enter bootload mode
SVL_CMD_NEXT    = 0x03  # request next chunk
SVL_CMD_FRAME   = 0x04  # indicate app data frame
SVL_CMD_RETRY   = 0x05  # request re-send frame
SVL_CMD_DONE    = 0x06  # finished - all data sent


# ***********************************************************************************
#
# Compute CRC on a byte array
#
# ***********************************************************************************
def get_crc16(data):
    # To perform the division perform the following:

    # Load the register with zero bits.
    # Augment the message by appending W zero bits to the end of it.
    # While (more message bits)
    #     Begin
    #     Shift the register left by one bit, reading the next bit of the
    #         augmented message into register bit position 0.
    #     If (a 1 bit popped out of the register during step 3)
    #         Register = Register XOR Poly.
    #     End
    # The register now contains the remainder.
    register    = 0x0000
    poly        = 0x8005

    data = bytearray(data)
    data.extend(bytearray(2))
    bits = 8*len(data)

    def get_data_bit(bit):
        byte = int(bit/8)
        if(data[byte] & (0x80 >> (bit%8))):
            return 1
        return 0

    for bit in range(bits):

        c = 0
        if(register & 0x8000):
            c = 1

        register <<= 1
        register &= 0xFFFF

        if(get_data_bit(bit)):
            register |= 0x0001

        if(c):
            register = (register ^ poly)

    return register



# ***********************************************************************************
#
# Wait for a packet 
#
# ***********************************************************************************
def wait_for_packet(ser):

    packet = {'len':0, 'cmd':0, 'data':0, 'crc':1, 'timeout':1}

    n = ser.read(2) # get the number of bytes
    if(len(n) < 2):
        return packet
    
    packet['len'] = int.from_bytes(n, byteorder='big', signed=False)    # 
    payload = ser.read(packet['len'])

    if(len(payload) != packet['len']):
        return packet
    
    packet['timeout'] = 0                           # all bytes received, so timeout is not true
    packet['cmd'] = payload[0]                      # cmd is the first byte of the payload
    packet['data'] = payload[1:packet['len']-2]     # the data is the part of the payload that is not cmd or crc
    packet['crc'] = get_crc16(payload)              # performing the crc on the whole payload should return 0

    return packet

# ***********************************************************************************
#
# Send a packet
#
# ***********************************************************************************
def send_packet(ser, cmd, data):
    data = bytearray(data)
    num_bytes = 3 + len(data)
    payload = bytearray(cmd.to_bytes(1,'big'))
    payload.extend(data)
    crc = get_crc16(payload)
    payload.extend(bytearray(crc.to_bytes(2,'big')))

    ser.write(num_bytes.to_bytes(2,'big'))
    ser.write(bytes(payload))






# ***********************************************************************************
#
# Setup: signal baud rate, get version, and command BL enter
#
# ***********************************************************************************
def phase_setup(ser):

    baud_detect_byte = b'U'

    verboseprint('\nphase:\tsetup')
    
                                            # Handle the serial startup blip
    ser.reset_input_buffer()
    verboseprint('\tcleared startup blip')         

    ser.write(baud_detect_byte)             # send the baud detection character

    packet = wait_for_packet(ser)
    if(packet['timeout'] or packet['crc']):
        return 1
    
    twopartprint('\t','Got SVL Bootloader Version: ' +
                 str(int.from_bytes(packet['data'], 'big')))
    verboseprint('\tSending \'enter bootloader\' command')

    send_packet(ser, SVL_CMD_BL, b'')

    # Now enter the bootload phase

    




# ***********************************************************************************
#
# Bootloader phase (Artemis is locked in)
#
# ***********************************************************************************
def phase_bootload(ser):

    frame_size = 512*4

    resend_max = 64
    resend_count = 0

    verboseprint('\nphase:\tbootload')

    with open(args.binfile, mode='rb') as binfile:
        application = binfile.read()
        total_len = len(application)

        total_frames = math.ceil(total_len/frame_size)
        curr_frame = 0

        verboseprint('\thave ' + str(total_len) + ' bytes to send in ' + str(total_frames) + ' frames')

        bl_done = False
        bl_failed = False
        while((not bl_done) and (not bl_failed)):
                
            packet = wait_for_packet(ser)               # wait for indication by Artemis
            if(packet['timeout'] or packet['crc']):
                print('\n\terror receiving packet')
                print(packet)
                print('\n')
                bl_failed = True
                bl_done = True

            if( packet['cmd'] == SVL_CMD_NEXT ):
                # verboseprint('\tgot frame request')
                curr_frame += 1
                resend_count = 0
            elif( packet['cmd'] == SVL_CMD_RETRY ):
                verboseprint('\t\tretrying...')
                resend_count += 1
                if( resend_count >= resend_max ):
                    bl_failed = True
                    bl_done = True
            else:
                print('unknown error')
                bl_failed = True
                bl_done = True

            if( curr_frame <= total_frames ):
                frame_data = application[((curr_frame-1)*frame_size):((curr_frame-1+1)*frame_size)]
                verboseprint('\tsending frame #'+str(curr_frame)+', length: '+str(len(frame_data)))

                send_packet(ser, SVL_CMD_FRAME, frame_data)

            else:
                send_packet(ser, SVL_CMD_DONE, b'')
                bl_done = True

        if( bl_failed == False ):
            twopartprint('\n\t', 'Upload complete')
        else:
            twopartprint('\n\t', 'Upload failed')

        return bl_failed






# ***********************************************************************************
#
# Help if serial port could not be opened
#
# ***********************************************************************************
def phase_serial_port_help():
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


# ***********************************************************************************
#
# Main function
#
# ***********************************************************************************
def main():
    try:
        num_tries = 3

        print('\n\nArtemis SVL Bootloader')

        for _ in range(num_tries):

            with serial.Serial(args.port, args.baud, timeout=args.timeout) as ser:

                t_su = 0.15             # startup time for Artemis bootloader   (experimentally determined - 0.095 sec min delay)

                time.sleep(t_su)        # Allow Artemis to come out of reset
                phase_setup(ser)        # Perform baud rate negotiation

                bl_failed = phase_bootload(ser)     # Bootload

            if( bl_failed == False ):
                break

    except:
        phase_serial_port_help()
    
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

    parser.add_argument("-t", "--timeout", default=0.50, help="Communication timeout in seconds (default 0.5)",
                         type=float)

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

    def twopartprint(verbosestr, printstr):
        if args.verbose:
            print(verbosestr, end = '')

        print(printstr)

    main()
