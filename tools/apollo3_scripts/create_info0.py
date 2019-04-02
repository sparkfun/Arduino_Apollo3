#!/usr/bin/env python3
# Utility to create info0 for Apollo3

import argparse
import sys
from Crypto.Cipher import AES
import array
import hashlib
import hmac
import os
import binascii

from am_defines import *
from keys_info import keyTblAes, keyTblHmac, custKey

def copy_keys(binarray, offset, key, size):
    for i in range (0, size):
        binarray[offset + i] = key[i]

#******************************************************************************
#
# Generate the info0 blob as per command line parameters
#
#******************************************************************************
def process(valid, version, output, mainPtr, secPol, keyWrap, secBoot, secBootOnRst, plOnExit, sDbg, bEnErase, infoProg, bNoSramWipe, bSwoCtrl, bDbgAllowed, custTrim, custTrim2, overrideGpio, overridePol, wiredIfMask, wiredSlvInt, wiredI2cAddr, wiredTimeout, u0, u1, u2, u3, u4, u5, krev, arev, chipId0, chipId1, sresv, wprot0, wprot1, rprot0, rprot1, swprot0, swprot1, srprot0, srprot1):

    #generate mutable byte array for the header
    hdr_binarray = bytearray([0xFF]*INFO_SIZE_BYTES);

    # initialize signature
    if (valid == 1):
        fill_word(hdr_binarray, INFO0_SIGNATURE0_O, AM_SECBOOT_INFO0_SIGN_PROGRAMMED0)
        fill_word(hdr_binarray, INFO0_SIGNATURE1_O, AM_SECBOOT_INFO0_SIGN_PROGRAMMED1)
        fill_word(hdr_binarray, INFO0_SIGNATURE2_O, AM_SECBOOT_INFO0_SIGN_PROGRAMMED2)
        fill_word(hdr_binarray, INFO0_SIGNATURE3_O, AM_SECBOOT_INFO0_SIGN_PROGRAMMED3)
    elif (valid == 0):
        fill_word(hdr_binarray, INFO0_SIGNATURE0_O, AM_SECBOOT_INFO0_SIGN_UINIT0)
        fill_word(hdr_binarray, INFO0_SIGNATURE1_O, AM_SECBOOT_INFO0_SIGN_UINIT1)
        fill_word(hdr_binarray, INFO0_SIGNATURE2_O, AM_SECBOOT_INFO0_SIGN_UINIT2)
        fill_word(hdr_binarray, INFO0_SIGNATURE3_O, AM_SECBOOT_INFO0_SIGN_UINIT3)
    else:
        fill_word(hdr_binarray, INFO0_SIGNATURE0_O, FLASH_INVALID)
        fill_word(hdr_binarray, INFO0_SIGNATURE1_O, FLASH_INVALID)
        fill_word(hdr_binarray, INFO0_SIGNATURE2_O, FLASH_INVALID)
        fill_word(hdr_binarray, INFO0_SIGNATURE3_O, FLASH_INVALID)

    am_print("info0 Signature...")
    am_print([hex(hdr_binarray[n]) for n in range(0, 16)])

    # Flash wipe is no longer supported
    bNoFlashWipe = 1
    if (valid == 1):
        # Build Security word
        blAtReset = 1
        if (secBoot == 0):
            secBoot = 0x5
        else:
            secBoot = 0x2
        if (secBootOnRst == 0):
            secBootOnRst = 0x5
        else:
            secBootOnRst = 0x2

        security = (((secPol & 0x7) << 24) | ((keyWrap & 0xf) << 20) | (secBootOnRst << 16) | (secBoot << 12) | ((plOnExit & 0x1) << 11) | ((sDbg & 0x1) << 10) | ((blAtReset & 0x1) << 9) | ((bEnErase & 0x1) << 8) | ((infoProg & 0xf) << 4) | ((bNoFlashWipe & 0x1) << 3) | ((bNoSramWipe & 0x1) << 2) | ((bSwoCtrl & 0x1) << 1) | ((bDbgAllowed & 0x1)))

        am_print("Security Word = ", hex(security))
        fill_word(hdr_binarray, INFO0_SECURITY_O, security)

        # Customer trim
        am_print("Customer Trim = ", hex(custTrim))
        fill_word(hdr_binarray, INFO0_CUSTOMER_TRIM_O, custTrim)

        am_print("Customer Trim2 = ", hex(custTrim2))
        fill_word(hdr_binarray, INFO0_CUSTOMER_TRIM2_O, custTrim2)

        # Override
        override = ((overridePol & 0x1) << 7) | (overrideGpio & 0x7f)
        am_print("Override = ", hex(override))
        fill_word(hdr_binarray, INFO0_SECURITY_OVR_O, override)

        # Wired interface config
        wired = ((wiredIfMask & 0x7) | ((wiredSlvInt & 0x3f) << 3) | ((wiredI2cAddr & 0x7f) << 9) | ((wiredTimeout & 0xffff) << 16))
        am_print("WiredCfg = ", hex(wired))
        fill_word(hdr_binarray, INFO0_SECURITY_WIRED_CFG_O, wired)

        # Wired UART cfg
        if (wiredIfMask & 0x1): # UART
            am_print("UART Config ", hex(u0), hex(u1), hex(u2), hex(u3), hex(u4), hex(u5))
            fill_word(hdr_binarray, INFO0_SECURITY_WIRED_IFC_CFG0_O, u0)
            fill_word(hdr_binarray, INFO0_SECURITY_WIRED_IFC_CFG1_O, u1)
            fill_word(hdr_binarray, INFO0_SECURITY_WIRED_IFC_CFG2_O, u2)
            fill_word(hdr_binarray, INFO0_SECURITY_WIRED_IFC_CFG3_O, u3)
            fill_word(hdr_binarray, INFO0_SECURITY_WIRED_IFC_CFG4_O, u4)
            fill_word(hdr_binarray, INFO0_SECURITY_WIRED_IFC_CFG5_O, u5)

        # version
        am_print("Version = ", hex(version))
        fill_word(hdr_binarray, INFO0_SECURITY_VERSION_O, version)

        # main ptr
        am_print("Main Ptr = ", hex(mainPtr))
        fill_word(hdr_binarray, INFO0_MAIN_PTR1_O, mainPtr)

        # SRAM Reservation
        am_print("SRAM Reservation = ", hex(sresv))
        fill_word(hdr_binarray, INFO0_SECURITY_SRAM_RESV_O, sresv)

        # Flash Protections
        am_print("Permanent Write Protections = ", hex(wprot0), ":", hex(wprot1))
        am_print("Permanent Copy Protections = ", hex(rprot0), ":", hex(rprot1))
        fill_word(hdr_binarray, INFO0_WRITE_PROTECT_L_O, wprot0)
        fill_word(hdr_binarray, INFO0_WRITE_PROTECT_H_O, wprot1)
        fill_word(hdr_binarray, INFO0_COPY_PROTECT_L_O, rprot0)
        fill_word(hdr_binarray, INFO0_COPY_PROTECT_H_O, rprot1)

        am_print("SBL Overridable Write Protections = ", hex(swprot0), ":", hex(swprot1))
        am_print("SBL Overridable Copy Protections = ", hex(srprot0), ":", hex(srprot1))
        fill_word(hdr_binarray, INFO0_WRITE_PROTECT_SBL_L_O, swprot0)
        fill_word(hdr_binarray, INFO0_WRITE_PROTECT_SBL_H_O, swprot1)
        fill_word(hdr_binarray, INFO0_COPY_PROTECT_SBL_L_O, srprot0)
        fill_word(hdr_binarray, INFO0_COPY_PROTECT_SBL_H_O, srprot1)

        # Customer Key
        am_print("Customer Key")
        am_print([hex(n) for n in custKey])
        copy_keys(hdr_binarray, INFO0_CUSTOMER_KEY0_O, custKey, 16)

        # Keys
        chipId = int_to_bytes(chipId0) + int_to_bytes(chipId1)
        while (len(chipId) != AM_SECBOOT_KEYIDX_BYTES):
            chipId = chipId + chipId
        am_print("chipID 16")
        am_print([hex(n) for n in chipId])

        kekOffset = INFO0_CUST_KEK_W0_O
        authKeyOffset = INFO0_CUST_AUTH_W0_O

        if (keyWrap == 0): ## None
            am_print("wrap mode 0")
            am_print("KEK")
            for i in range (0, INFO_MAX_ENC_KEYS):
                am_print([hex(n) for n in keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]])
                copy_keys(hdr_binarray, kekOffset + i*AM_SECBOOT_KEYIDX_BYTES, keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)], AM_SECBOOT_KEYIDX_BYTES)
            am_print("AuthKey")
            for i in range (0, INFO_MAX_AUTH_KEYS):
                am_print([hex(n) for n in keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]])
                copy_keys(hdr_binarray, authKeyOffset + i*AM_SECBOOT_KEYIDX_BYTES, keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)], AM_SECBOOT_KEYIDX_BYTES)
        elif (keyWrap == 1): ## XOR
            am_print("wrap mode 1")
            am_print("KEK")
            for i in range (0, INFO_MAX_ENC_KEYS):
                scrambledKey = keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]
                for j in range (0, AM_SECBOOT_KEYIDX_BYTES):
                    scrambledKey[j] = scrambledKey[j] ^ custKey[j] ^ chipId[j]
                am_print([hex(n) for n in scrambledKey])
                copy_keys(hdr_binarray, kekOffset + i*AM_SECBOOT_KEYIDX_BYTES, scrambledKey, AM_SECBOOT_KEYIDX_BYTES)
            am_print("AuthKey")
            for i in range (0, INFO_MAX_AUTH_KEYS):
                scrambledKey = keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]
                for j in range (0, AM_SECBOOT_KEYIDX_BYTES):
                    scrambledKey[j] = scrambledKey[j] ^ custKey[j] ^ chipId[j]
                am_print([hex(n) for n in scrambledKey])
                copy_keys(hdr_binarray, authKeyOffset + i*AM_SECBOOT_KEYIDX_BYTES, scrambledKey, AM_SECBOOT_KEYIDX_BYTES)
        elif (keyWrap == 2): ## AES128
            am_print("wrap mode 2")
            am_print("KEK")
            for i in range (0, INFO_MAX_ENC_KEYS):
                scrambledKey = encrypt_app_aes(keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)], custKey, chipId)
                am_print([hex(n) for n in scrambledKey])
                copy_keys(hdr_binarray, kekOffset + i*AM_SECBOOT_KEYIDX_BYTES, scrambledKey, AM_SECBOOT_KEYIDX_BYTES)
            am_print("AuthKey")
            for i in range (0, INFO_MAX_AUTH_KEYS):
                scrambledKey = encrypt_app_aes(keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)], custKey, chipId)
                am_print([hex(n) for n in scrambledKey])
                copy_keys(hdr_binarray, authKeyOffset + i*AM_SECBOOT_KEYIDX_BYTES, scrambledKey, AM_SECBOOT_KEYIDX_BYTES)
        else:
            am_print("Unknown wrap mode")

        # Revocation Masks
        am_print("KREV Mask = ", hex(krev))
        fill_word(hdr_binarray, INFO0_KREVTRACK_O, krev)
        am_print("AREV Mask = ", hex(arev))
        fill_word(hdr_binarray, INFO0_AREVTRACK_O, arev)


    # now output all three binary arrays in the proper order
    with open(output + '.bin', mode = 'wb') as out:
        am_print("Writing to file ", output + '.bin')
        out.write(hdr_binarray)

def parse_arguments():
    parser = argparse.ArgumentParser(description =
                     'Generate Corvette Info0 Blob')

    parser.add_argument('output',
                        help = 'Output filename (without the extension)')

    parser.add_argument('--valid', dest = 'valid', type=auto_int, default=1, choices = [0,1,2],
                        help = 'INFO0 Valid 0 = Uninitialized, 1 = Valid, 2 = Invalid (Default = 1)?')

    parser.add_argument('--version', dest = 'version', type=auto_int, default=0,
                        help = 'version (Default = 0)?')

    parser.add_argument('--main', dest = 'mainPtr', type=auto_int, default=hex(AM_SECBOOT_DEFAULT_NONSECURE_MAIN),
                        help = 'Main Firmware location (Default = ' + str(hex(AM_SECBOOT_DEFAULT_NONSECURE_MAIN)) + ')?')

    parser.add_argument('--secpol', dest = 'secPol', type=auto_int, default=hex(0x0), choices = [0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7],
                        help = 'Security Policy Bitmask (Default = 0)? (bit 0 = Auth, bit 1 = Enc, bit 2 = Version Rollback)')

    parser.add_argument('--wrap', dest = 'keyWrap', type=auto_int, default=0, choices = [0,1,2],
                        help = 'KeyWrap Algo (Default = 0)? (0 = none, 1 = XOR, 2 = AES128)')

    parser.add_argument('--sRst', dest = 'secBootOnRst', type=auto_int, default=0, choices = [0,1],
                        help = 'Secure Boot on Soft Reset (Default = 0) ?')

    parser.add_argument('-s', dest = 'secBoot', type=auto_int, default=0, choices = [0,1],
                        help = 'Secure Boot (Default = 0) ?')

    parser.add_argument('--pl', dest = 'plOnExit', type=auto_int, default=0, choices = [0,1],
                        help = 'Protection Lock Enabled (Default = 0) ?')

    parser.add_argument('--sDbgAllowed', dest = 'sDbg', type=auto_int, default=1, choices = [0,1],
                        help = 'Debugger allowed during (optional) Secondary Bootloader (Default = 1) ?')

    parser.add_argument('--erase', dest = 'bEnErase', type=auto_int, default=1, choices = [0,1],
                        help = 'Info0 Erase Allowed (Default = 1) ?')

    parser.add_argument('--prog', dest = 'infoProg', type=auto_int, default=hex(0xf),
                        help = 'INFO0 Program allowed (1 bit per quadrant) (Default = 0xf) ?')

    parser.add_argument('--snowipe', dest = 'bNoSramWipe', type=auto_int, default=1, choices = [0,1],
                        help = ' Do not wipe SRAM on debugger connection (Default = 1) ?')

    parser.add_argument('--swo', dest = 'bSwoCtrl', type=auto_int, default=1, choices = [0,1],
                        help = 'debugger connection allowed (Default = 1) ?')

    parser.add_argument('--dbgprot', dest = 'bDbgAllowed', type=auto_int, default=1, choices = [0,1],
                        help = 'Do not lock debugger (Default = 1) ?')

    parser.add_argument('--trim', dest = 'custTrim', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'customer trim ?')

    parser.add_argument('--trim2', dest = 'custTrim2', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'customer trim 2?')

    parser.add_argument('--gpio', dest = 'overrideGpio', type=auto_int, default=hex(0x7f),
                        help = 'Override GPIO (7 bit - in hex) - 0x7f for disabled (Default = 0x7f)')

    parser.add_argument('--gpiolvl', dest = 'overridePol', type=auto_int, default=0, choices = [0,1],
                        help = 'Override GPIO Polarity (0 = low, 1 = hi) (Default = 0)')

    parser.add_argument('--wmask', dest = 'wiredIfMask', type=auto_int, default=0x1,
                        help = 'Wired interface mask (bit 0 = UART, bit 1 = SPI, bit 2 = I2C) (default = UART)')

    parser.add_argument('--wSlInt', dest = 'wiredSlvInt', type=auto_int, default=4,
                        help = 'Wired IOS interface handshake pin (default = 4)')

    parser.add_argument('--wI2c', dest = 'wiredI2cAddr', type=auto_int, default=hex(0x20),
                        help = 'Wired IOS interface I2C Address (default = 0x20)')

    parser.add_argument('--wTO', dest = 'wiredTimeout', type=auto_int, default=20000,
                        help = 'Wired interface timeout in millisec (default = 20000)')

    parser.add_argument('--u0', dest = 'u0', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'UART Config 0 (default = 0xFFFFFFFF)')

    parser.add_argument('--u1', dest = 'u1', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'UART Config 1 (default = 0xFFFFFFFF)')

    parser.add_argument('--u2', dest = 'u2', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'UART Config 2 (default = 0xFFFFFFFF)')

    parser.add_argument('--u3', dest = 'u3', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'UART Config 3 (default = 0xFFFFFFFF)')

    parser.add_argument('--u4', dest = 'u4', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'UART Config 4 (default = 0xFFFFFFFF)')

    parser.add_argument('--u5', dest = 'u5', type=auto_int, default=hex(0xFFFFFFFF),
                        help = 'UART Config 5 (default = 0xFFFFFFFF)')

    parser.add_argument('--krev', dest='krev', type=auto_int, default = hex(0xFFFFFFFF),
                        help='KEK Revocation Mask (Default 0xFFFFFFFF)')

    parser.add_argument('--arev', dest='arev', type=auto_int, default = hex(0xFFFFFFFF),
                        help='AuthKey Revocation Mask (Default 0xFFFFFFFF)')

    parser.add_argument('--sresv', dest='sresv', type=auto_int, default = hex(0x0),
                        help='SRAM Reservation (Default 0x0)')

    parser.add_argument('--chipid0', dest='chipId0', type=auto_int, default = 0,
                        help='CHIPID0 for the device (Default 0)')

    parser.add_argument('--chipid1', dest='chipId1', type=auto_int, default = 0,
                        help='CHIPID1 for the device (Default 0)')

    parser.add_argument('--wprot0', dest='wprot0', type=auto_int, default = hex(0xFFFFFFFF),
                        help='Permanent Write Protections Mask for flash#0 (Default 0xFFFFFFFF)')

    parser.add_argument('--wprot1', dest='wprot1', type=auto_int, default = hex(0xFFFFFFFF),
                        help='Permanent Write Protections Mask for flash#1 (Default 0xFFFFFFFF)')

    parser.add_argument('--rprot0', dest='rprot0', type=auto_int, default = hex(0xFFFFFFFF),
                        help='Permanent Copy Protections Mask for flash#0 (Default 0xFFFFFFFF)')

    parser.add_argument('--rprot1', dest='rprot1', type=auto_int, default = hex(0xFFFFFFFF),
                        help='Permanent Copy Protections Mask for flash#1 (Default 0xFFFFFFFF)')

    parser.add_argument('--swprot0', dest='swprot0', type=auto_int, default = hex(0xFFFFFFFF),
                        help='SBL overridable Write Protections Mask for flash#0 (Default 0xFFFFFFFF)')

    parser.add_argument('--swprot1', dest='swprot1', type=auto_int, default = hex(0xFFFFFFFF),
                        help='SBL overridable Write Protections Mask for flash#1 (Default 0xFFFFFFFF)')

    parser.add_argument('--srprot0', dest='srprot0', type=auto_int, default = hex(0xFFFFFFFF),
                        help='SBL overridable Copy Protections Mask for flash#0 (Default 0xFFFFFFFF)')

    parser.add_argument('--srprot1', dest='srprot1', type=auto_int, default = hex(0xFFFFFFFF),
                        help='SBL overridable Copy Protections Mask for flash#1 (Default 0xFFFFFFFF)')

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

    process(args.valid, args.version, args.output, args.mainPtr, args.secPol, args.keyWrap, args.secBoot, args.secBootOnRst, args.plOnExit, args.sDbg, args.bEnErase, args.infoProg, args.bNoSramWipe, args.bSwoCtrl, args.bDbgAllowed, args.custTrim, args.custTrim2, args.overrideGpio, args.overridePol, args.wiredIfMask, args.wiredSlvInt, args.wiredI2cAddr, args.wiredTimeout, args.u0, args.u1, args.u2, args.u3, args.u4, args.u5, args.krev, args.arev, args.chipId0, args.chipId1, args.sresv, args.wprot0, args.wprot1, args.rprot0, args.rprot1, args.swprot0, args.swprot1, args.srprot0, args.srprot1)

if __name__ == '__main__':
    main()
