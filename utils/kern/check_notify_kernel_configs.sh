#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

echo "====> $KERNEL_DIR/$KERNEL_CFG"
grep -iP 'config_.*notify' "$KERNEL_DIR/$KERNEL_CFG"
