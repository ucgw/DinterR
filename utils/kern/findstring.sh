#!/bin/sh
function help {
    >&2 echo "Usage: $0 <grep string>"
    exit 1
}

if [ -z $1 ];then
    help
fi

grep_string=$1
source `pwd`/dinterbuild.env 2>/dev/null || exit 1

exclude_dirs="$KERNEL_DIR/drivers"
exclude_files="$KERNEL_DIR/.*\.o\.cmd"

find $KERNEL_DIR -type f 2>/dev/null | grep -vP "$exclude_dirs" | grep -vP "$exclude_files" | xargs grep -iP "$grep_string" | grep -viP '^Binary file'
