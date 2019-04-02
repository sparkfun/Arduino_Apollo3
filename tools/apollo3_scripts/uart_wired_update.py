#!/usr/bin/env python3
# UART wired update host for Corvette Bootloader

import argparse
import serial
import sys
import array
import os
import binascii

from am_defines import *

#******************************************************************************
#
# Main function
#
#******************************************************************************
def main():

    # Open a serial port, and communicate with Device
    #
    # We will use a UART timeout value of 12 second. This should be long
    # enough that the slave will respond to us, but short enough that a human
    # operator should see errors quickly..
    # Max flashing time depends on the amount of SRAM available.
    # For very large images, the flashing happens page by page.
    # However if the image can fit in the free SRAM, it could take a long time
    # for the whole image to be flashed at the end.
    # The largest image which can be stored depends on the max SRAM.
    # Assuming worst case ~100 ms/page of flashing time, and allowing for the
    # image to be close to occupying full SRAM (256K) which is 128 pages.

    print('Connecting with Corvette over serial port {}...'.format(args.port), flush=True)

    with serial.Serial(args.port, args.baud, timeout=12) as ser:
        connect_device(ser)

    print('Done.')

#******************************************************************************
#
# Communicate with Device
#
# Given a serial port, connects to the target device using the
# UART.
#
#******************************************************************************
def connect_device(ser):

    # Send Hello
    #generate mutable byte array for the header
    hello = bytearray([0x00]*4);
    
    fill_word(hello, 0, ((8 << 16) | AM_SECBOOT_WIRED_MSGTYPE_HELLO))
    print('Sending Hello.')
    response = send_command(hello, 88, ser)
    print("Received response for Hello")
    word = word_from_bytes(response, 4)
    if ((word & 0xFFFF) == AM_SECBOOT_WIRED_MSGTYPE_STATUS):
        # Received Status
        print("Received Status")
        print("length = ", hex((word >> 16)))
        print("version = ", hex(word_from_bytes(response, 8)))
        print("Max Storage = ", hex(word_from_bytes(response, 12)))
        print("Status = ", hex(word_from_bytes(response, 16)))
        print("State = ", hex(word_from_bytes(response, 20)))
        print("AMInfo = ")
        for x in range(24, 88, 4):
            print(hex(word_from_bytes(response, x)))

        abort = args.abort
        if (abort != -1):
            # Send OTA Desc
            print('Sending Abort command.')
            abortMsg = bytearray([0x00]*8);
            fill_word(abortMsg, 0, ((12 << 16) | AM_SECBOOT_WIRED_MSGTYPE_ABORT))
            fill_word(abortMsg, 4, abort)
            send_ackd_command(abortMsg, ser)

        otadescaddr = args.otadesc
        if (otadescaddr != 0xFFFFFFFF):
            # Send OTA Desc
            print('Sending OTA Descriptor = ', hex(otadescaddr))
            otaDesc = bytearray([0x00]*8);
            fill_word(otaDesc, 0, ((12 << 16) | AM_SECBOOT_WIRED_MSGTYPE_OTADESC))
            fill_word(otaDesc, 4, otadescaddr)
            send_ackd_command(otaDesc, ser)

        imageType = args.imagetype
        if (args.binfile != ''):

            # Read the binary file from the command line.
            with open(args.binfile, mode='rb') as binfile:
                application = binfile.read()
            # Gather the important binary metadata.
            totalLen = len(application)
            # Send Update command
            print('Sending Update Command.')

            # It is assumed that maxSize is 256b multiple
            maxImageSize = args.split
            if ((maxImageSize & (FLASH_PAGE_SIZE - 1)) != 0):
                print ("split needs to be multiple of flash page size")
                return

            # Each Block of image consists of AM_WU_IMAGEHDR_SIZE Bytes Image header and the Image blob
            maxUpdateSize = AM_WU_IMAGEHDR_SIZE + maxImageSize
            numUpdates = (totalLen + maxUpdateSize - 1) // maxUpdateSize # Integer division
            print("number of updates needed = ", numUpdates)

            end = totalLen
            for numUpdates in range(numUpdates, 0 , -1):
                start = (numUpdates-1)*maxUpdateSize
                crc = crc32(application[start:end])
                applen = end - start
                print("Sending block of size ", str(hex(applen)), " from ", str(hex(start)), " to ", str(hex(end)))
                end = end - applen

                update = bytearray([0x00]*16);
                fill_word(update, 0, ((20 << 16) | AM_SECBOOT_WIRED_MSGTYPE_UPDATE))
                fill_word(update, 4, applen)
                fill_word(update, 8, crc)
                # Size = 0 => We're not piggybacking any data to IMAGE command
                fill_word(update, 12, 0)

                send_ackd_command(update, ser)

                # Loop over the bytes in the image, and send them to the target.
                resp = 0
                # Max chunk size is AM_MAX_UART_MSG_SIZE adjusted for the header for Data message
                maxChunkSize = AM_MAX_UART_MSG_SIZE - 12
                for x in range(0, applen, maxChunkSize):
                    # Split the application into chunks of maxChunkSize bytes.
                    # This is the max chunk size supported by the UART bootloader
                    if ((x + maxChunkSize) > applen):
                        chunk = application[start+x:start+applen]
#                        print(str(hex(start+x)), " to ", str(hex(applen)))
                    else:
                        chunk = application[start+x:start+x+maxChunkSize]
#                        print(str(hex(start+x)), " to ", str(hex(start + x + maxChunkSize)))

                    chunklen = len(chunk)

                    # Build a data packet with a "data command" a "length" and the actual
                    # payload bytes, and send it to the target.
                    dataMsg = bytearray([0x00]*8);
                    fill_word(dataMsg, 0, (((chunklen + 12) << 16) | AM_SECBOOT_WIRED_MSGTYPE_DATA))
                    # seqNo
                    fill_word(dataMsg, 4, x)

                    print("Sending Data Packet of length ", chunklen)
                    send_ackd_command(dataMsg + chunk, ser)

        if (args.raw != ''):

            # Read the binary file from the command line.
            with open(args.raw, mode='rb') as rawfile:
                blob = rawfile.read()
            # Send Raw command
            print('Sending Raw Command.')
            ser.write(blob)

        if (args.reset != 0):
            # Send reset
            print('Sending Reset Command.')
            resetmsg = bytearray([0x00]*8);
            fill_word(resetmsg, 0, ((12 << 16) | AM_SECBOOT_WIRED_MSGTYPE_RESET))
            # options
            fill_word(resetmsg, 4, args.reset)
            send_ackd_command(resetmsg, ser)
    else:
        # Received Wrong message
        print("Received Unknown Message")
        word = word_from_bytes(response, 4)
        print("msgType = ", hex(word & 0xFFFF))
        print("Length = ", hex(word >> 16))
        print([hex(n) for n in response])
        print("!!!Wired Upgrade Unsuccessful!!!....Terminating the script")
        exit()

#******************************************************************************
#
# Send ACK'd command
#
# Sends a command, and waits for an ACK.
#
#******************************************************************************
def send_ackd_command(command, ser):
    response = send_command(command, 20, ser)

    word = word_from_bytes(response, 4)
    if ((word & 0xFFFF) == AM_SECBOOT_WIRED_MSGTYPE_ACK):
        # Received ACK
        if (word_from_bytes(response, 12) != AM_SECBOOT_WIRED_ACK_STATUS_SUCCESS):
            print("Received NACK")
            print("msgType = ", hex(word_from_bytes(response, 8)))
            print("error = ", hex(word_from_bytes(response, 12)))
            print("seqNo = ", hex(word_from_bytes(response, 16)))
            print("!!!Wired Upgrade Unsuccessful!!!....Terminating the script")
            exit()

    return response

#******************************************************************************
#
# Send command
#
# Sends a command, and waits for the response.
#
#******************************************************************************
def send_command(params, response_len, ser):

    # Compute crc
    crc = crc32(params)
#    print([hex(n) for n in int_to_bytes(crc)])
#    print([hex(n) for n in params])
    # send crc first
    ser.write(int_to_bytes(crc))

    # Next, send the parameters.
    ser.write(params)

    response = ''
    response = ser.read(response_len)

    # Make sure we got the number of bytes we asked for.
    if len(response) != response_len:
        print('No response for command 0x{:08X}'.format(word_from_bytes(params, 0) & 0xFFFF))
        n = len(response)
        if (n != 0):
            print("received bytes ", len(response))
            print([hex(n) for n in response])
        raise NoResponseError

    return response

#******************************************************************************
#
# Send a command that uses an array of bytes as its parameters.
#
#******************************************************************************
def send_bytewise_command(command, params, response_len, ser):
    # Send the command first.
    ser.write(int_to_bytes(command))

    # Next, send the parameters.
    ser.write(params)

    response = ''
    response = ser.read(response_len)

    # Make sure we got the number of bytes we asked for.
    if len(response) != response_len:
        print('No response for command 0x{:08X}'.format(command))
        raise NoResponseError

    return response

#******************************************************************************
#
# Errors
#
#******************************************************************************
class BootError(Exception):
    pass

class NoAckError(BootError):
    pass

class NoResponseError(BootError):
    pass

#******************************************************************************
#
# Main program flow
#
#******************************************************************************
if __name__ == '__main__':

    parser = argparse.ArgumentParser(description =
                                     'UART Wired Update Host for Apollo3')

    parser.add_argument('port', help = 'Serial COMx Port')

    parser.add_argument('-b', dest='baud', default=115200, type=int,
                        help = 'Baud Rate (default is 115200)')

    parser.add_argument('--raw', dest='raw', default='',
                        help = 'Binary file for raw message')

    parser.add_argument('-f', dest='binfile', default='',
                        help = 'Binary file to program into the target device')

    parser.add_argument('-i', dest = 'imagetype', default=AM_SECBOOT_WIRED_IMAGETYPE_INVALID, type=auto_int,
                        choices = [
                                (AM_SECBOOT_WIRED_IMAGETYPE_SBL),
                                (AM_SECBOOT_WIRED_IMAGETYPE_AM3P),
                                (AM_SECBOOT_WIRED_IMAGETYPE_PATCH),
                                (AM_SECBOOT_WIRED_IMAGETYPE_MAIN),
                                (AM_SECBOOT_WIRED_IMAGETYPE_CHILD),
                                (AM_SECBOOT_WIRED_IMAGETYPE_CUSTPATCH),
                                (AM_SECBOOT_WIRED_IMAGETYPE_NONSECURE),
                                (AM_SECBOOT_WIRED_IMAGETYPE_INFO0),
                                (AM_SECBOOT_WIRED_IMAGETYPE_INFO0_NOOTA),
                                (AM_SECBOOT_WIRED_IMAGETYPE_INVALID)
                                ],
                        help = 'ImageType ('
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_SBL) + ': SBL, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_AM3P) + ': AM3P, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_PATCH) + ': Patch, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_MAIN) + ': Main, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_CHILD) + ': Child, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_CUSTPATCH) + ': CustOTA, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_NONSECURE) + ': NonSecure, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_INFO0) + ': Info0 '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_INFO0_NOOTA) + ': Info0_NOOTA) '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_INVALID) + ': Invalid) '
                                '- default[Invalid]')

    parser.add_argument('-o', dest = 'otadesc', type=auto_int, default=0xFE000,
                        help = 'OTA Descriptor Page address (hex) - (Default is 0xFE000 - at the end of main flash) - enter 0xFFFFFFFF to instruct SBL to skip OTA')

    parser.add_argument('-r', dest = 'reset', default=1, type=auto_int, choices = [0,1,2],
                        help = 'Should it send reset command after image download? (0 = no reset, 1 = POI, 2 = POR) (default is 1)')

    parser.add_argument('-a', dest = 'abort', default=-1, type=int, choices = [0,1,-1],
                        help = 'Should it send abort command? (0 = abort, 1 = abort and quit, -1 = no abort) (default is -1)')

    parser.add_argument('--split', dest='split', type=auto_int, default=hex(MAX_DOWNLOAD_SIZE),
                        help='Specify the max block size if the image will be downloaded in pieces')

    args = parser.parse_args()

    main()
