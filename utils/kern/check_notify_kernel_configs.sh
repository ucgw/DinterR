#!/bin/sh
KERNEL_VER=4.19.143
KERNEL_DIR="$HOME/csce499/linux-$KERNEL_VER"
KERNEL_CFG=.config

echo "====> $KERNEL_DIR/$KERNEL_CFG"
grep -iP 'config_.*notify' "$KERNEL_DIR/$KERNEL_CFG"
