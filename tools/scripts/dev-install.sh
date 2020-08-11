#!/bin/sh

echo 'Apollo3 Arduino Core Dev Install'
arch=packages/SparkFun/hardware/apollo3

echo 'Determining latest version'
versions=$(ls $arch | tr ' ' '\n' | sort --version-sort | tr '\n' ' ')
latest=''
for version in $versions
do
  latest=$version
done
if [ -z "$latest" ]; then
  echo 'Could not determine latest version number - exiting'
  exit
fi

echo $latest

echo 'Removing versions'
rm -rf $arch

echo 'Cloning latest'
mkdir -p $arch
git clone https://github.com/sparkfun/Arduino_Apollo3 $arch/$latest

echo 'Done'
