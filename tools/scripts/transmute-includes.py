#!/usr/bin/env python

import argparse
import glob


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

    with open(args.dest, 'w') as fout:
        for path in includes.split("\" \""):
            old = '-Imbed-os'
            if not old in path:
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