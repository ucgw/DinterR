#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1
dt_snap=`date +"%Y%m%d%H%M%S"`

for ORIGFILE in `find $KERNEL_DIR -name "*.orig" 2>/dev/null`;do
  SRCFILE=`echo $ORIGFILE | sed -e 's/\.orig//g'`
  BASENAME_SRCFILE=`basename $SRCFILE`
  PATCHFILE="$BASENAME_SRCFILE#$dt_snap.patch"
  diff_output=`diff -u $ORIGFILE $SRCFILE`
  if [ "$?" == "1" ];then
      echo "$diff_output" > "$PATCH_DIR/$PATCHFILE"
  fi
done
