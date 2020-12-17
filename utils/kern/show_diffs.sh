#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

for ORIGFILE in `find $KERNEL_DIR -name "*.orig" 2>/dev/null`;do
  SRCFILE=`echo $ORIGFILE | sed -e 's/\.orig//g'`
  diff_output=`diff -u $ORIGFILE $SRCFILE`
  if [ "$?" == "1" ];then
      echo "$diff_output"
  fi
done
