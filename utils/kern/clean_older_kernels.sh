#!/bin/sh
#
# quick script to clean up older kernel image(s)
# (including init ramdisk images)
# not currently configured in .config
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

case "$1" in
  dry)
    DRYRUN=1
    ;;
  run)
    DRYRUN=0
    ;;
  *)
    echo "Usage: $0 (run|dry)"
    exit 1;
    ;;
esac

BOOTDIR=/boot
MODULESDIR=/usr/lib/modules
INITRAMFS_PREFIX="initramfs-$KERNEL_VER"
SYSTEMMAP_PREFIX="System.map-$KERNEL_VER"
VMLINUZ_PREFIX="vmlinuz-$KERNEL_VER"
MODULES_PREFIX="$CURRENT_CONFIG_LOCALVERSION_VALUE"

CURRENT_INITRAMFS_VERSION_REGEX="$INITRAMFS_PREFIX${CURRENT_CONFIG_LOCALVERSION_VALUE}"
CURRENT_SYSTEMMAP_VERSION_REGEX="$SYSTEMMAP_PREFIX${CURRENT_CONFIG_LOCALVERSION_VALUE}"
CURRENT_VMLINUZ_VERSION_REGEX="$VMLINUZ_PREFIX${CURRENT_CONFIG_LOCALVERSION_VALUE}"
CURRENT_MODULES_VERSION_REGEX="${KERNEL_VER}${MODULES_PREFIX}"

for OLDER in `ls $BOOTDIR/$VMLINUZ_PREFIX* 2>/dev/null;ls $BOOTDIR/$SYSTEMMAP_PREFIX* 2>/dev/null;ls $BOOTDIR/$INITRAMFS_PREFIX* 2>/dev/null`;do
  if [[ ! $OLDER =~ $CURRENT_INITRAMFS_VERSION_REGEX ]] && \
     [[ ! $OLDER =~ $CURRENT_SYSTEMMAP_VERSION_REGEX ]] && \
     [[ ! $OLDER =~ $CURRENT_VMLINUZ_VERSION_REGEX ]]; then
    if [ "$DRYRUN" == "1" ];then
      echo "would execute: 'sudo rm $OLDER'"
    else
      echo "deleting $OLDER"
      sudo rm $OLDER
    fi
  fi
done

for OLDER in `ls $MODULESDIR 2>/dev/null`;do
  if [[ $OLDER =~ $KERNEL_VER ]] && \
     [[ ! $OLDER =~ $CURRENT_MODULES_VERSION_REGEX ]]; then
    if [ "$DRYRUN" == "1" ];then
      echo "would execute: 'sudo rm -rf $MODULESDIR/$OLDER'"
    else
      echo "deleting $MODULESDIR/$OLDER"
      sudo rm -rf $MODULESDIR/$OLDER
    fi
  fi
done

if [ "$DRYRUN" == "1" ];then
  echo "would execute: 'sudo grub2-mkconfig -o /boot/grub2/grub.cfg'"
else
  echo "reconfiguring grub"
  sudo grub2-mkconfig -o /boot/grub2/grub.cfg
fi
