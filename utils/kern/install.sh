#!/bin/sh
KERNEL_VER=4.19.143
KERNEL_DIR="$HOME/csce499/linux-$KERNEL_VER"

echo "====> Installing Linux Kernel+Modules: $KERNEL_VER"
cd $KERNEL_DIR
sudo make modules_install && sudo make install
