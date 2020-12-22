#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1
changed_files=""
crafted_find="find $KERNEL_DIR"

for SRCFILE in `$crafted_find -name "*.orig" 2>/dev/null`;do
  FILENAME=`echo $SRCFILE | sed -e 's/\.orig//g'`
  changed_files+=" $FILENAME"
done

# apply mtime ordering
ls -t $changed_files
