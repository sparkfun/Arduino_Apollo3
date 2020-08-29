#!/usr/bin/env python
 
# This file is subject to the terms and conditions defined in
# file 'LICENSE.md', which is part of this source code package.

import argparse
import glob
import json

# ***********************************************************************************
#
# Main function
#
# ***********************************************************************************
def main():
    print("transmuting includes")

    matches = glob.glob(args.include)

    print('\tinclude: ' + matches[0])
    print('\tdestination: ' + args.dest)

    with open(matches[0], 'r') as fin:
        includes = fin.read()

    includes = includes.lstrip('"')
    includes = includes.rstrip('"')

    blocklist = []
    if args.blocklist:
        print(args.blocklist)
        try:
            with open(args.blocklist, 'r') as fin:
                data = json.loads(fin.read())
                blocklist = data["includes"]
            print('\tusing user-specified blocklist')
            verboseprint('\t\t' + str(data))
        except FileNotFoundError as e:
            print(e)
        except:
            print('error loading the blocklist file')

    with open(args.dest, 'w') as fout:
        for path in includes.split("\" \""):
            old = '-Imbed-os'
            if not old in path:
                continue
        
            blocked = False
            for blockpath in blocklist:
                if path == '-Imbed-os/' + blockpath:
                    blocked = True
                    print('blocked: ' + path)
                    break
            if blocked:
                continue

            transmuted = '"' + path.replace(old, '-iwithprefixbeforembed-os', 1) + '"'
            verboseprint('\t' + transmuted)
            fout.write(transmuted + '\n')

    exit()



# ******************************************************************************
#
# Main program flow
#
# ******************************************************************************
if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        description='Utility to generate GCC flag and symbol files from mbed build profiles')

    parser.add_argument('-i', '--include', dest='include', required=True, help='glob pattern matching includes file to convert')
    parser.add_argument('-d', '--dest', dest='dest', required=True, help='path to output file')
    parser.add_argument('-b', '--blocklist', dest='blocklist', help='path to JSON formatted blocklist of paths to exclude from output - relative to mbed-os')
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