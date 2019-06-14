#!/usr/bin/env python3
# Utility to create image blobs for Corvette Secure Boot

import argparse
import sys
from Crypto.Cipher import AES
import array
import hashlib
import hmac
import os
import binascii

from am_defines import *
from keys_info import keyTblAes, keyTblHmac, minAesKeyIdx, maxAesKeyIdx, minHmacKeyIdx, maxHmacKeyIdx, INFO_KEY


#******************************************************************************
#
# Generate the image blob as per command line parameters
#
#******************************************************************************
def process(loadaddress, appFile, magicNum, crcI, crcB, authI, authB, protection, authKeyIdx, output, encKeyIdx, version, erasePrev, child0, child1, authalgo, encalgo):

    app_binarray = bytearray()
    # Open the file, and read it into an array of integers.
    with appFile as f_app:
        app_binarray.extend(f_app.read())
        f_app.close()

    encVal = 0
    if (encalgo != 0):
        encVal = 1
        if ((encKeyIdx < minAesKeyIdx) or (encKeyIdx > maxAesKeyIdx)):
            am_print("Invalid encKey Idx ", encKeyIdx, level=AM_PRINT_LEVEL_ERROR);
            return
        if (encalgo == 2):
            if (encKeyIdx & 0x1):
                am_print("Invalid encKey Idx ", encKeyIdx, level=AM_PRINT_LEVEL_ERROR);
                return
            keySize = 32
        else:
            keySize = 16
    if (authalgo != 0):
        if ((authKeyIdx < minHmacKeyIdx) or (authKeyIdx > maxHmacKeyIdx) or (authKeyIdx & 0x1)):
            am_print("Invalid authKey Idx ", authKeyIdx, level=AM_PRINT_LEVEL_ERROR);
            return

    if (magicNum == AM_IMAGE_MAGIC_MAIN):
        hdr_length  = AM_IMAGEHDR_SIZE_MAIN;   #fixed header length
    elif ((magicNum == AM_IMAGE_MAGIC_CHILD) or (magicNum == AM_IMAGE_MAGIC_CUSTPATCH) or (magicNum == AM_IMAGE_MAGIC_NONSECURE) or (magicNum == AM_IMAGE_MAGIC_INFO0)):
        hdr_length  = AM_IMAGEHDR_SIZE_AUX;   #fixed header length
    else:
        am_print("magic number", hex(magicNum), " not supported", level=AM_PRINT_LEVEL_ERROR)
        return
    am_print("Header Size = ", hex(hdr_length))

    #generate mutable byte array for the header
    hdr_binarray = bytearray([0x00]*hdr_length);

    orig_app_length  = (len(app_binarray))
    am_print("original app_size ",hex(orig_app_length), "(",orig_app_length,")")

    am_print("load_address ",hex(loadaddress), "(",loadaddress,")")
    if (loadaddress & 0x3):
        am_print("load address needs to be word aligned", level=AM_PRINT_LEVEL_ERROR)
        return

    if (magicNum == AM_IMAGE_MAGIC_INFO0):
        if (orig_app_length & 0x3):
            am_print("INFO0 blob length needs to be multiple of 4", level=AM_PRINT_LEVEL_ERROR)
            return
        if ((loadaddress + orig_app_length) > INFO_SIZE_BYTES):
            am_print("INFO0 Offset and length exceed size", level=AM_PRINT_LEVEL_ERROR)
            return

    if (encVal == 1):
        block_size = AM_SECBOOT_AESCBC_BLOCK_SIZE_BYTES
        app_binarray = pad_to_block_size(app_binarray, block_size, 1)
    else:
        # Add Padding
        app_binarray = pad_to_block_size(app_binarray, 4, 0)
    
    app_length  = (len(app_binarray))
    am_print("app_size ",hex(app_length), "(",app_length,")")

    # Create Image blobs

    # w0
    blobLen = hdr_length + app_length
    w0 = (magicNum << 24) | ((encVal & 0x1) << 23) | blobLen

    am_print("w0 =", hex(w0))
    fill_word(hdr_binarray, 0, w0)
        
    # w2
    securityVal = ((authI << 1) | crcI) << 4 | (authB << 1) | crcB
    am_print("Security Value ", hex(securityVal))
    w2 = ((securityVal << 24) & 0xff000000) | ((authalgo) & 0xf) | ((authKeyIdx << 4) & 0xf0) | ((encalgo << 8) & 0xf00) | ((encKeyIdx << 12) & 0xf000)
    fill_word(hdr_binarray, 8, w2)
    am_print("w2 = ",hex(w2))
    

    if (magicNum == AM_IMAGE_MAGIC_INFO0):
        # Insert the INFO0 size and offset
        addrWord = ((orig_app_length>>2) << 16) | ((loadaddress>>2) & 0xFFFF)
        versionKeyWord = INFO_KEY
    else:
        # Insert the application binary load address.
        addrWord = loadaddress | (protection & 0x3)
        # Initialize versionKeyWord
        versionKeyWord = (version & 0x7FFF) | ((erasePrev & 0x1) << 15)

    am_print("addrWord = ",hex(addrWord))
    fill_word(hdr_binarray, AM_IMAGEHDR_OFFSET_ADDR, addrWord)

    am_print("versionKeyWord = ",hex(versionKeyWord))
    fill_word(hdr_binarray, AM_IMAGEHDR_OFFSET_VERKEY, versionKeyWord)

    # Initialize child (Child Ptr/ Feature key)
    am_print("child0/feature = ",hex(child0))
    fill_word(hdr_binarray, AM_IMAGEHDR_OFFSET_CHILDPTR, child0)
    am_print("child1 = ",hex(child1))
    fill_word(hdr_binarray, AM_IMAGEHDR_OFFSET_CHILDPTR + 4, child1)

    authKeyIdx = authKeyIdx - minHmacKeyIdx
    if (authB != 0): # Authentication needed
        am_print("Boot Authentication Enabled")
#        am_print("Key used for HMAC")
#        am_print([hex(keyTblHmac[authKeyIdx*AM_SECBOOT_KEYIDX_BYTES + n]) for n in range (0, AM_HMAC_SIG_SIZE)])
        # Initialize the clear image HMAC
        sigClr = compute_hmac(keyTblHmac[authKeyIdx*AM_SECBOOT_KEYIDX_BYTES:(authKeyIdx*AM_SECBOOT_KEYIDX_BYTES+AM_HMAC_SIG_SIZE)], (hdr_binarray[AM_IMAGEHDR_START_HMAC:hdr_length] + app_binarray))
        am_print("HMAC Clear")
        am_print([hex(n) for n in sigClr])
        # Fill up the HMAC
        for x in range(0, AM_HMAC_SIG_SIZE):
            hdr_binarray[AM_IMAGEHDR_OFFSET_SIGCLR + x]  = sigClr[x]

    # All the header fields part of the encryption are now final
    if (encVal == 1):
        am_print("Encryption Enabled")
        encKeyIdx = encKeyIdx - minAesKeyIdx
        ivValAes = os.urandom(AM_SECBOOT_AESCBC_BLOCK_SIZE_BYTES)
        am_print("Initialization Vector")
        am_print([hex(ivValAes[n]) for n in range (0, AM_SECBOOT_AESCBC_BLOCK_SIZE_BYTES)])
        keyAes = os.urandom(keySize)
        am_print("AES Key used for encryption")
        am_print([hex(keyAes[n]) for n in range (0, keySize)])
        # Encrypted Part
        am_print("Encrypting blob of size " , (hdr_length - AM_IMAGEHDR_START_ENCRYPT + app_length))
        enc_binarray = encrypt_app_aes((hdr_binarray[AM_IMAGEHDR_START_ENCRYPT:hdr_length] + app_binarray), keyAes, ivValAes)
#        am_print("Key used for encrypting AES Key")
#        am_print([hex(keyTblAes[encKeyIdx*keySize + n]) for n in range (0, keySize)])
        # Encrypted Key
        enc_key = encrypt_app_aes(keyAes, keyTblAes[encKeyIdx*keySize:encKeyIdx*keySize + keySize], ivVal0)
        am_print("Encrypted Key")
        am_print([hex(enc_key[n]) for n in range (0, keySize)])
        # Fill up the IV
        for x in range(0, AM_SECBOOT_AESCBC_BLOCK_SIZE_BYTES):
            hdr_binarray[AM_IMAGEHDR_OFFSET_IV + x]  = ivValAes[x]
        # Fill up the Encrypted Key
        for x in range(0, keySize):
            hdr_binarray[AM_IMAGEHDR_OFFSET_KEK + x]  = enc_key[x]
    else:
        enc_binarray = hdr_binarray[AM_IMAGEHDR_START_ENCRYPT:hdr_length] + app_binarray


    if (authI != 0): # Install Authentication needed
        am_print("Install Authentication Enabled")
#        am_print("Key used for HMAC")
#        am_print([hex(keyTblHmac[authKeyIdx*AM_SECBOOT_KEYIDX_BYTES + n]) for n in range (0, AM_HMAC_SIG_SIZE)])
        # Initialize the top level HMAC
        sig = compute_hmac(keyTblHmac[authKeyIdx*AM_SECBOOT_KEYIDX_BYTES:(authKeyIdx*AM_SECBOOT_KEYIDX_BYTES+AM_HMAC_SIG_SIZE)], (hdr_binarray[AM_IMAGEHDR_START_HMAC_INST:AM_IMAGEHDR_START_ENCRYPT] + enc_binarray))
        am_print("Generated Signature")
        am_print([hex(n) for n in sig])
        # Fill up the HMAC
        for x in range(0, AM_HMAC_SIG_SIZE):
            hdr_binarray[AM_IMAGEHDR_OFFSET_SIG + x]  = sig[x]
    # compute the CRC for the blob - this is done on a clear image
    crc = crc32(hdr_binarray[AM_IMAGEHDR_START_CRC:hdr_length] + app_binarray)
    am_print("crc =  ",hex(crc));
    w1 = crc
    fill_word(hdr_binarray, AM_IMAGEHDR_OFFSET_CRC, w1)

    # now output all three binary arrays in the proper order
    output = output + '.bin'
    am_print("Writing to file ", output)
    with open(output, mode = 'wb') as out:
        out.write(hdr_binarray[0:AM_IMAGEHDR_START_ENCRYPT])
        out.write(enc_binarray)



def parse_arguments():
    parser = argparse.ArgumentParser(description =
                     'Generate Corvette Image Blob')

    parser.add_argument('--bin', dest='appFile', type=argparse.FileType('rb'),
                        help='binary file (blah.bin)')
                        
    parser.add_argument('--load-address', dest='loadaddress', type=auto_int, default=hex(AM_SECBOOT_DEFAULT_NONSECURE_MAIN),
                        help='Load address of the binary.')

    parser.add_argument('--magic-num', dest='magic_num', default=hex(AM_IMAGE_MAGIC_NONSECURE),
                        type=lambda x: x.lower(),
#                        type = str.lower,
                        choices = [
                                hex(AM_IMAGE_MAGIC_MAIN),
                                hex(AM_IMAGE_MAGIC_CHILD),
                                hex(AM_IMAGE_MAGIC_CUSTPATCH),
                                hex(AM_IMAGE_MAGIC_NONSECURE),
                                hex(AM_IMAGE_MAGIC_INFO0)
                                ],
                        help = 'Magic Num ('
                                + str(hex(AM_IMAGE_MAGIC_MAIN)) + ': Main, '
                                + str(hex(AM_IMAGE_MAGIC_CHILD)) + ': Child, '
                                + str(hex(AM_IMAGE_MAGIC_CUSTPATCH)) + ': CustOTA, '
                                + str(hex(AM_IMAGE_MAGIC_NONSECURE)) + ': NonSecure, '
                                + str(hex(AM_IMAGE_MAGIC_INFO0)) + ': Info0) '
                                '- default[Main]'
                                )
                                                    
    parser.add_argument('-o', dest = 'output', default='outimage',
                        help = 'Output filename (without the extension)')

    parser.add_argument('--authkey', dest = 'authkey', type=auto_int, default=(minHmacKeyIdx), choices = range(minHmacKeyIdx, maxHmacKeyIdx + 1),
                        help = 'Authentication Key Idx? (' + str(minHmacKeyIdx) + ' to ' + str(maxHmacKeyIdx) + ')')

    parser.add_argument('--kek', dest = 'kek', type=auto_int, default=(minAesKeyIdx), choices = range(minAesKeyIdx, maxAesKeyIdx+1),
                        help = 'KEK index? (' + str(minAesKeyIdx) + ' to ' + str(maxAesKeyIdx) + ')')

    parser.add_argument('--authalgo', dest = 'authalgo', type=auto_int, default=0, choices=range(0, AM_SECBOOT_AUTH_ALGO_MAX+1),
                        help = helpAuthAlgo)

    parser.add_argument('--encalgo', dest = 'encalgo', type=auto_int, default=0, choices = range(0, AM_SECBOOT_ENC_ALGO_MAX+1),
                        help = helpEncAlgo)

    parser.add_argument('--child0', dest = 'child0', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'child (blobPtr#0 for Main / feature key for AM3P)')

    parser.add_argument('--child1', dest = 'child1', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'child (blobPtr#1 for Main)')

    parser.add_argument('--version', dest = 'version', type=auto_int, default=0,
                        help = 'version (15 bit)')

    parser.add_argument('--crcI', dest = 'crcI', type=auto_int, default=1, choices=[0,1],
                        help = 'Install CRC check enabled (Default = Y)?')

    parser.add_argument('--crcB', dest = 'crcB', type=auto_int, default=0, choices=[0,1],
                        help = 'Boot CRC check enabled (Default = N)?')

    parser.add_argument('--authI', dest = 'authI', type=auto_int, default=0, choices=[0,1],
                        help = 'Install Authentication check enabled (Default = N)?')

    parser.add_argument('--authB', dest = 'authB', type=auto_int, default=0, choices=[0,1],
                        help = 'Boot Authentication check enabled (Default = N)?')

    parser.add_argument('--erasePrev', dest = 'erasePrev', type=auto_int, default=0, choices=[0,1],
                        help = 'erasePrev (Valid only for main)')

    parser.add_argument('-p', dest = 'protection', type=auto_int, default=0, choices = [0x0, 0x1, 0x2, 0x3],
                        help = 'protection info 2 bit C W')

    parser.add_argument('--loglevel', dest='loglevel', type=auto_int, default=AM_PRINT_LEVEL_INFO,
                        choices = range(AM_PRINT_LEVEL_MIN, AM_PRINT_LEVEL_MAX+1),
                        help=helpPrintLevel)


    args = parser.parse_args()
    args.magic_num = int(args.magic_num, 16)


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


    process(args.loadaddress, args.appFile, args.magic_num, args.crcI, args.crcB, args.authI, args.authB, args.protection, args.authkey, args.output, args.kek, args.version, args.erasePrev, args.child0, args.child1, args.authalgo, args.encalgo)

if __name__ == '__main__':
    main()
