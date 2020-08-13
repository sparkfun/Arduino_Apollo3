#!/usr/bin/env python

# This file is subject to the terms and conditions defined in
# file 'LICENSE.md', which is part of this source code package.

import argparse
import json

# ***********************************************************************************
#
# Main function
#
# ***********************************************************************************
def main():
    print("transmuting profiles")

    print('\tprofile: ' + args.profile)
    print('\tdestination: ' + args.dest)

    with open(args.profile, 'r') as fin:
        profile = json.loads(fin.read())

    blocklist = json.loads(b'{}')
    if args.blocklist:
        try:
            with open(args.blocklist, 'r') as fin:
                blocklist = json.loads(fin.read())['profiles'][args.type]
            print('\tusing user-specified blocklist')
            verboseprint('\t\t' + str(blocklist))
        except FileNotFoundError as e:
            print(e)
        except:
            print('error loading the blocklist file')

    try:
        blocklist['flags']
    except KeyError:
        blocklist['flags'] = []
    try:
        blocklist['macros']
    except KeyError:
        blocklist['macros'] = []
    try:
        blocklist['symbols']
    except KeyError:
        blocklist['symbols'] = []

    outpath = args.dest + args.type + '-flags'
    verboseprint('\twriting into ' + outpath)
    with open(outpath, 'w') as fout:
        for flag in profile['flags']:
            if flag in blocklist['flags']:
                continue
            output = flag + ' '
            verboseprint('\t\t' + output)
            fout.write(output)

    outpath = args.dest + args.type + '-macros'
    verboseprint('\twriting into ' + outpath)
    with open(outpath, 'w') as fout:
        for macro in profile['macros']:
            if macro in blocklist['macros']:
                continue
            output = macro
            verboseprint('\t\t' + output)
            fout.write(output)

    outpath = args.dest + args.type + '-symbols'
    verboseprint('\twriting into ' + outpath)
    with open(outpath, 'w') as fout:
        for symbol in profile['symbols']:
            if symbol in blocklist['symbols']:
                continue
            output = '-D' + symbol + ' '
            verboseprint('\t\t' + output)
            fout.write(output)

    exit()



# ******************************************************************************
#
# Main program flow
#
# ******************************************************************************
if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        description='Utility to generate GCC flag and symbol files from mbed build profiles')

    parser.add_argument('-p', '--profile', dest='profile', required=True, help='path to profile to convert')
    parser.add_argument('-d', '--dest', dest='dest', required=True, help='path to output files - will be postfixed with extension')
    parser.add_argument('-t', '--type', dest='type', required=True, help='type of profile ("cxx", "c", "asm" or "ld")')
    parser.add_argument('-b', '--blocklist', dest='blocklist', help='path to JSON formatted blocklist for with a "profiles" document with sub documents for each type each with subdocuemts for "flags", "symbols" and "macros")')
    parser.add_argument('-v', '--verbose', default=0, help='enable verbose output', action='store_true')

    args = parser.parse_args()

    # Create print function for verbose output if caller deems it: https://stackoverflow.com/questions/5980042/how-to-implement-the-verbose-or-v-option-into-a-script
    if args.verbose:
        def verboseprint(*args):
            # Print each argument separately so caller doesn't need to
            # stuff everything to be printed into a single string
            for arg in args:
                print(arg, end='', flush=True)
            print()
    else:
        verboseprint = lambda *a: None      # do-nothing function

    def twopartprint(verbosestr, printstr):
        if args.verbose:
            print(verbosestr, end = '')

        print(printstr)

    main()