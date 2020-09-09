#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

echo "KERNEL_VER='$KERNEL_VER'"
echo "KERNEL_DIR='$KERNEL_DIR'"
echo "KERNEL_CFG='$KERNEL_CFG'"
echo "LOCAL_VERSION_KEY='$LOCAL_VERSION_KEY'"
echo "LOCAL_VERSION_REGEX='$LOCAL_VERSION_REGEX'"
echo "LOCAL_VERSION_INIT_NUMBER=$LOCAL_VERSION_INIT_NUMBER"
echo "LOCAL_VERSION_INIT_SUBSTRING='$LOCAL_VERSION_INIT_SUBSTRING'"
echo "CURRENT_CONFIG_LOCALVERSION='$CURRENT_CONFIG_LOCALVERSION'"
echo "CURRENT_CONFIG_LOCALVERSION_VALUE='$CURRENT_CONFIG_LOCALVERSION_VALUE'"

