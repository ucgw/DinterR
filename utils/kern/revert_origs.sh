#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1
revert_success=0

for SRCFILE in `find $KERNEL_DIR -name "*.orig" 2>/dev/null`;do
  MD5_SRC=`md5sum $SRCFILE | awk '{ print \$1 }'`
  UPDATEDFILE=`echo $SRCFILE | sed -e 's/\.orig//g'`
  cp $SRCFILE $UPDATEDFILE
  MD5_DST=`md5sum $UPDATEDFILE | awk '{ print \$1 }'`
  if [ $MD5_SRC == $MD5_DST ];then
      >&2 echo "$UPDATEDFILE reverted to orignal"
  else
      >&2 echo "$UPDATEDFILE *NOT* reverted to original! orig: $MD5_SRC   updated: $MD5_DST"
      revert_success=1
  fi
done

exit $revert_success
