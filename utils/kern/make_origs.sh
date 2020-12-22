#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1
orig_success=0
crafted_find="find $KERNEL_DIR"

for SRCFILE in `$crafted_find -name "*.*~" 2>/dev/null`;do
  FILENAME=`echo $SRCFILE | sed -e 's/\.~.*$//g'`
  cp $SRCFILE ${FILENAME}.orig
  if [ $? -eq 0 ];then
      >&2 echo "${FILENAME}.orig created"
  else
      >&2 echo "${FILENAME}.orig *NOT* created"
      orig_success=1
  fi
done

exit $orig_success
