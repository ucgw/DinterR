#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

echo "====> Installing Linux Kernel+Modules: $KERNEL_VER"
cd $KERNEL_DIR
sudo make modules_install && sudo make install
