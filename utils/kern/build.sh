#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

if [ -e $KERNEL_DIR ];then
  echo "====> Building Linux Kernel: $KERNEL_VER"
  cd $KERNEL_DIR
  make -j $(nproc)
else
  exit 1
fi
