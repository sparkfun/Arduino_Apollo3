#!/usr/bin/env python3

import argparse
import sys
from Crypto.Cipher import AES
import array
import os
import binascii

from am_defines import *
from keys_info import keyTblAes, keyTblHmac, wrapKey, minWrapMode


#******************************************************************************
#
# Generate the device keys for a given chipId based on well known keys and key Wrap mode
#
#******************************************************************************
def process(wrapMode, chipId0, chipId1, inFile):
    if (inFile is not None):
        for line in inFile:
            linecontent = line.rstrip()
            if linecontent:
                chipids = line.split(':')
                process_chip(wrapMode, int(chipids[0], 16), int(chipids[1], 16))
    else:
        process_chip(wrapMode, chipId0, chipId1)

def process_chip(wrapMode, chipId0, chipId1):

    output = str(hex(chipId0)) + '_' + str(hex(chipId1))
    chipId = int_to_bytes(chipId0) + int_to_bytes(chipId1)
    while (len(chipId) != AM_SECBOOT_KEYIDX_BYTES):
        chipId = chipId + chipId
    am_print("ChipID: ", [hex(n) for n in chipId], level=AM_PRINT_LEVEL_VERBOSE)

    if (wrapMode == AM_SECBOOT_KEYWRAP_NONE): ## None
        am_print("wrap mode None")
        with open(output + '_info_kek.bin', mode = 'wb') as out_info_kek:
            am_print("writing to " + output + '_info_kek.bin')
            for i in range (0, INFO_MAX_ENC_KEYS):
                am_print([hex(n) for n in keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]], level=AM_PRINT_LEVEL_DEBUG)
                out_info_kek.write(bytearray(keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]))
        with open(output + '_info_authkey.bin', mode = 'wb') as out_info_authkey:
            am_print("writing to " + output + '_info_authkey.bin')
            for i in range (0, INFO_MAX_AUTH_KEYS):
                am_print([hex(n) for n in keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]], level=AM_PRINT_LEVEL_DEBUG)
                out_info_authkey.write(bytearray(keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]))
    elif (wrapMode == AM_SECBOOT_KEYWRAP_XOR): ## XOR
        am_print("wrap mode XOR")
        with open(output + '_info_kek.bin', mode = 'wb') as out_info_kek:
            am_print("writing to " + output + '_info_kek.bin')
            for i in range (0, INFO_MAX_ENC_KEYS):
                scrambledKey = keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]
                for j in range (0, AM_SECBOOT_KEYIDX_BYTES):
                    scrambledKey[j] = scrambledKey[j] ^ wrapKey[j] ^ chipId[j]
                am_print([hex(n) for n in scrambledKey], level=AM_PRINT_LEVEL_DEBUG)
                out_info_kek.write(bytearray(scrambledKey))
        with open(output + '_info_authkey.bin', mode = 'wb') as out_info_authkey:
            am_print("writing to " + output + '_info_authkey.bin')
            for i in range (0, INFO_MAX_AUTH_KEYS):
                scrambledKey = keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]
                for j in range (0, AM_SECBOOT_KEYIDX_BYTES):
                    scrambledKey[j] = scrambledKey[j] ^ wrapKey[j] ^ chipId[j]
                am_print([hex(n) for n in scrambledKey], level=AM_PRINT_LEVEL_DEBUG)
                out_info_authkey.write(bytearray(scrambledKey))
    elif (wrapMode == AM_SECBOOT_KEYWRAP_AES128): ## AES128
        am_print("wrap mode AES128")
        with open(output + '_info_kek.bin', mode = 'wb') as out_info_kek:
            am_print("writing to " + output + '_info_kek.bin')
            for i in range (0, INFO_MAX_ENC_KEYS):
                scrambledKey = encrypt_app_aes(keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)], wrapKey, chipId)
                am_print([hex(n) for n in scrambledKey], level=AM_PRINT_LEVEL_DEBUG)
                out_info_kek.write(bytearray(scrambledKey))
        with open(output + '_info_authkey.bin', mode = 'wb') as out_info_authkey:
            am_print("writing to " + output + '_info_authkey.bin')
            for i in range (0, INFO_MAX_AUTH_KEYS):
                scrambledKey = encrypt_app_aes(keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)], wrapKey, chipId)
                am_print([hex(n) for n in scrambledKey], level=AM_PRINT_LEVEL_DEBUG)
                out_info_authkey.write(bytearray(scrambledKey))
    else:
        am_print("Unknown wrap mode", level=AM_PRINT_LEVEL_ERROR)
        return
        

def parse_arguments():
    parser = argparse.ArgumentParser(description =
                     'Generate Corvette Device keys')

    parser.add_argument('--wrap', dest='wrap', type=auto_int, default=minWrapMode, choices = range(minWrapMode, AM_SECBOOT_KEYWRAP_MAX+1),
                        help='wrap mode (0 = None, 1 = XOR, 2 = AES128) [default ' + str(minWrapMode) +'] ')
                        
    parser.add_argument('--chipid0', dest='chipId0', type=auto_int, default=0,
                        help='CHIPID0 for the device')
 
    parser.add_argument('--chipid1', dest='chipId1', type=auto_int, default=0,
                        help='CHIPID1 for the device')
 
    parser.add_argument('-f', type=argparse.FileType('r'), dest='inFile', nargs='?',
                        help='Input text file with chip ID Tuples (chipID0:chipID1 in each line)')
 
    parser.add_argument('--loglevel', dest='loglevel', type=auto_int, default=AM_PRINT_LEVEL_INFO,
                        choices = range(AM_PRINT_LEVEL_MIN, AM_PRINT_LEVEL_MAX+1),
                        help=helpPrintLevel)
 
    args = parser.parse_args()

    return args


#******************************************************************************
#
# Main function.
#
#******************************************************************************
def main():
    # Read the arguments.
    args = parse_arguments()

    am_set_print_level(args.loglevel)
    process(args.wrap, args.chipId0, args.chipId1, args.inFile)

if __name__ == '__main__':
    main()
