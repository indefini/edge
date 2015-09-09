#!/bin/sh

release=$(pkg-config --variable=release enlightenment)
#MODULE_ARCH="$host_os-$host_cpu-$release"
cpu=$(uname -m)
MODULE_ARCH="linux-gnu-$cpu-$release"
#echo $MODULE_ARCH
root=$HOME/.e/e/modules/edge
rootmodule=$root/$MODULE_ARCH
mkdir -p $rootmodule
cp module.desktop $root/
cp target/debug/libmodule.so $rootmodule/module.so

