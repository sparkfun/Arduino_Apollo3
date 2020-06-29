#!/usr/bin/env python

import argparse
import json

blacklist_format = '{"flags": [], "macros": [], "symbols": []}'

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

    blacklist = json.loads(blacklist_format)
    if args.blacklist:
        try:
            with open(args.blacklist, 'r') as fin:
                blacklist = json.loads(fin.read())
            print('\tusing user-specified blacklist')
            verboseprint('\t\t' + str(blacklist))
        except FileNotFoundError as e:
            print(e)
        except:
            print('error loading the blacklist file')

    try:
        blacklist['flags']
    except KeyError:
        blacklist['flags'] = []
    try:
        blacklist['macros']
    except KeyError:
        blacklist['macros'] = []
    try:
        blacklist['symbols']
    except KeyError:
        blacklist['symbols'] = []

    outpath = args.dest + '-flags'
    verboseprint('\twriting into ' + outpath)
    with open(outpath, 'w') as fout:
        for flag in profile['flags']:
            if flag in blacklist['flags']:
                continue
            output = flag + ' '
            verboseprint('\t\t' + output)
            fout.write(output)

    outpath = args.dest + '-macros'
    verboseprint('\twriting into ' + outpath)
    with open(outpath, 'w') as fout:
        for macro in profile['macros']:
            if macro in blacklist['macros']:
                continue
            output = macro
            verboseprint('\t\t' + output)
            fout.write(output)

    outpath = args.dest + '-symbols'
    verboseprint('\twriting into ' + outpath)
    with open(outpath, 'w') as fout:
        for symbol in profile['symbols']:
            if symbol in blacklist['symbols']:
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
    parser.add_argument('-b', '--blacklist', dest='blacklist', help='path to JSON formatted blacklist for ' + blacklist_format)
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