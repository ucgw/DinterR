#!/bin/sh
KERNEL_VER=4.19.143
KERNEL_DIR="$HOME/csce499/linux-$KERNEL_VER"

echo "====> Building Linux Kernel: $KERNEL_VER"
cd $KERNEL_DIR
make -j $(nproc)
