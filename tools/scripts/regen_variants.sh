#!/usr/bin/env bash

# use this script to update variants based on bsp files

# setup
set -e
set -o errexit
echo "" 1>&2

# get enclosing directory
DIR=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

# defaults
BSP_ROOT=.
BOARDS_FILE=$DIR/arduino_boards.sh
VARIANTS_ROOT=../../variants

# handle arguments
while getopts ":r:b:" opt; do
  case $opt in
    r) BSP_ROOT="$OPTARG"
    ;;
    b) BOARDS_FILE="$OPTARG"
    ;;
    \?) echo "Invalid option -$OPTARG" 1>&2
    ;;
  esac
done

# verify bsp root
echo "Using \$BSP_ROOT=$BSP_ROOT" 1>&2
VFILE=$BSP_ROOT/README.md
if [ -f "$VFILE" ]; 
then
    echo "\$BSP_ROOT verification passed" 1>&2
else
    echo "\$BSP_ROOT verification failed" 1>&2
    exit 1
fi

# load in boards to handle
echo "Using \$BOARDS_FILE=$BOARDS_FILE" 1>&2
source $BOARDS_FILE

# copy bsp files from BSP repo to Arduino variants
echo "" 1>&2
for value in $BOARDS
do
    echo "Copying bsp files for: $value" 1>&2
    cp $BSP_ROOT/$value/bsp/am_bsp_pins.c $VARIANTS_ROOT/$value/bsp/am_bsp_pins.c
    cp $BSP_ROOT/$value/bsp/am_bsp_pins.h $VARIANTS_ROOT/$value/bsp/am_bsp_pins.h
    cp $BSP_ROOT/$value/bsp/am_bsp.c $VARIANTS_ROOT/$value/bsp/am_bsp.c
    cp $BSP_ROOT/$value/bsp/am_bsp.h $VARIANTS_ROOT/$value/bsp/am_bsp.h
done