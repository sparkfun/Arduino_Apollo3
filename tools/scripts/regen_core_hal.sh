#!/usr/bin/env bash

# use this script to update the core hal based on a new release of the SDK
# setup
set -e
set -o errexit
echo "" 1>&2

# get enclosing directory
DIR=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

# defaults
SDK_ROOT=.
CORE_SDK_ROOT=$DIR/../../cores/arduino/am_sdk_ap3

# handle arguments
while getopts ":r:b:" opt; do
  case $opt in
    r) SDK_ROOT="$OPTARG"
    ;;
    \?) echo "Invalid option -$OPTARG" 1>&2
    ;;
  esac
done

# verify SDK root
echo "Using \$SDK_ROOT=$SDK_ROOT" 1>&2
VFILE=$SDK_ROOT/AM-BSD-EULA.txt
if [ -f "$VFILE" ]; 
then
    echo "\$SDK_ROOT verification passed" 1>&2
else
    echo "\$SDK_ROOT verification failed" 1>&2
    exit 1
fi

# clean out old stuff
rm -rf $CORE_SDK_ROOT/CMSIS
rm -rf $CORE_SDK_ROOT/devices
rm -rf $CORE_SDK_ROOT/mcu
rm -rf $CORE_SDK_ROOT/utils

# copy over new stuff
mkdir -p $CORE_SDK_ROOT/CMSIS/{AmbiqMicro/{Include,Source},ARM}
mkdir -p $CORE_SDK_ROOT/mcu/apollo3
mkdir -p $CORE_SDK_ROOT/devices

cp -r $SDK_ROOT/CMSIS/AmbiqMicro/Include/apollo3.h $CORE_SDK_ROOT/CMSIS/AmbiqMicro/Include/
cp -r $SDK_ROOT/CMSIS/AmbiqMicro/Include/apollo3c.h $CORE_SDK_ROOT/CMSIS/AmbiqMicro/Include/
cp -r $SDK_ROOT/CMSIS/AmbiqMicro/Include/system_apollo3.h $CORE_SDK_ROOT/CMSIS/AmbiqMicro/Include/
cp -r $SDK_ROOT/CMSIS/AmbiqMicro/Include/system_apollo3c.h $CORE_SDK_ROOT/CMSIS/AmbiqMicro/Include/
cp -r $SDK_ROOT/CMSIS/AmbiqMicro/Source/startup_apollo3.s $CORE_SDK_ROOT/CMSIS/AmbiqMicro/Source/
cp -r $SDK_ROOT/CMSIS/AmbiqMicro/Source/system_apollo3.c $CORE_SDK_ROOT/CMSIS/AmbiqMicro/Source/
cp -r $SDK_ROOT/CMSIS/ARM $CORE_SDK_ROOT/CMSIS/

cp -r $SDK_ROOT/devices/am_devices_button.h $CORE_SDK_ROOT/devices
cp -r $SDK_ROOT/devices/am_devices_button.c $CORE_SDK_ROOT/devices
cp -r $SDK_ROOT/devices/am_devices_led.h $CORE_SDK_ROOT/devices
cp -r $SDK_ROOT/devices/am_devices_led.c $CORE_SDK_ROOT/devices

cp -r $SDK_ROOT/mcu/apollo3/hal $CORE_SDK_ROOT/mcu/apollo3
cp -r $SDK_ROOT/mcu/apollo3/regs $CORE_SDK_ROOT/mcu/apollo3
cp -r $SDK_ROOT/mcu/apollo3/am_mcu_apollo.h $CORE_SDK_ROOT/mcu/apollo3

cp -r $SDK_ROOT/utils $CORE_SDK_ROOT