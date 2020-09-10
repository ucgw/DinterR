#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

if [ -e $KERNEL_DIR/$KERNEL_CFG ];then
  echo "====> $KERNEL_DIR/$KERNEL_CFG"
  grep -iP 'config_.*notify' "$KERNEL_DIR/$KERNEL_CFG"
else
  exit 1
fi
