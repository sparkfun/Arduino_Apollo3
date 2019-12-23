#!/usr/bin/env bash

# use this script to update the artemis SVL based on the SparkFun AmbiqSuite BSP files

# setup
set -e
set -o errexit
echo "" 1>&2

# get enclosing directory
DIR=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

# defaults
BSP_ROOT=.
ARTEMIS_TOOLS_ROOT=../artemis

# handle arguments
while getopts ":r:b:" opt; do
  case $opt in
    r) BSP_ROOT="$OPTARG"
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


# copy bsp files from BSP repo to Arduino variants
echo "" 1>&2

echo "Removing SVL files" 1>&2
rm -rf $ARTEMIS_TOOLS_ROOT/linux
rm -rf $ARTEMIS_TOOLS_ROOT/macosx
rm -rf $ARTEMIS_TOOLS_ROOT/windows
rm -f $ARTEMIS_TOOLS_ROOT/artemis_svl.py

echo "Copying SVL files" 1>&2
cp -r $BSP_ROOT/common/tools_sfe/artemis/linux $ARTEMIS_TOOLS_ROOT/linux
cp -r $BSP_ROOT/common/tools_sfe/artemis/macosx $ARTEMIS_TOOLS_ROOT/macosx
cp -r $BSP_ROOT/common/tools_sfe/artemis/windows $ARTEMIS_TOOLS_ROOT/windows
cp $BSP_ROOT/common/tools_sfe/artemis/artemis_svl.py $ARTEMIS_TOOLS_ROOT/artemis_svl.py