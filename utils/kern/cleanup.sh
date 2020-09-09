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
INITRAMFS_PREFIX="initramfs-$KERNEL_VER"
SYSTEMMAP_PREFIX="System.map-$KERNEL_VER"
VMLINUZ_PREFIX="vmlinuz-$KERNEL_VER"

CURRENT_INITRAMFS_VERSION_REGEX="$INITRAMFS_PREFIX${CURRENT_CONFIG_LOCALVERSION_VALUE}"
CURRENT_SYSTEMMAP_VERSION_REGEX="$SYSTEMMAP_PREFIX${CURRENT_CONFIG_LOCALVERSION_VALUE}"
CURRENT_VMLINUZ_VERSION_REGEX="$VMLINUZ_PREFIX${CURRENT_CONFIG_LOCALVERSION_VALUE}"

for OLDER in `ls $BOOTDIR/$VMLINUZ_PREFIX*;ls $BOOTDIR/$SYSTEMMAP_PREFIX*;ls $BOOTDIR/$INITRAMFS_PREFIX*`;do
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

if [ "$DRYRUN" == "1" ];then
  echo "would execute: 'sudo grub2-mkconfig -o /boot/grub2/grub.cfg'"
else
  echo "reconfiguring grub"
  sudo grub2-mkconfig -o /boot/grub2/grub.cfg
fi
