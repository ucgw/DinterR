#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

for SRCFILE in `find $KERNEL_DIR -name "*.orig" 2>/dev/null`;do
  P_SRCFILE_BASE=`echo $SRCFILE | sed -e 's/\.orig//g'`
  rm -f `ls $P_SRCFILE_BASE.~* 2>/dev/null`
done
