#!/usr/bin/env python
# File Output Tool for Apollo3 Arduino Core
# Allows users to export compiled executables in a variety of formats

# ***********************************************************************************
#
# Imports
#
# ***********************************************************************************

import argparse
import shutil
import sys
from sys import exit

# ***********************************************************************************
#
# Main function
#
# ***********************************************************************************
def main(args):
    print("\n\nApollo3 Arduino Executable Exporter")

    if args.format == "axf":
        sourcepath = args.axfpath
    elif args.format == "bin":
        sourcepath = args.binpath
    else:
        print('"' + str(args.format) + '" is not a valid format - exit')
        exit()

    shutil.copy2(sourcepath, args.sketchpath)
    print("exported " + str(args.axfpath) + " to " + str(args.sketchpath))

    exit()


# ******************************************************************************
#
# Main program flow
#
# ******************************************************************************
if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Apollo3 Executable Exporter")

    parser.add_argument(
        "-s",
        "--sketchpath",
        dest="sketchpath",
        help="path to sketch folder (file will be saved there)",
    )
    parser.add_argument("-b", "--binpath", dest="binpath", help="path to .bin file")
    parser.add_argument("-a", "--axfpath", dest="axfpath", help="path to .axf file")
    parser.add_argument(
        "-f",
        "--format",
        dest="format",
        default="axf",
        help="which format to export (axf, bin)",
    )

    args = parser.parse_args()

    main(args)
