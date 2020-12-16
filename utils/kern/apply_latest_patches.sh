#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1
patchfile_regex='([a-zA-Z0-9\_\-\.]+)\#([0-9]+)\.'

curr_source_code=''
latest_version=1
latest_patch=''
patch_init=''
patches_to_apply=()

for PATCH in `ls $PATCH_DIR`;do
    [[ $PATCH =~ $patchfile_regex ]]
    source_code=${BASH_REMATCH[1]}
    if [ "X$source_code" != "X" ];then
        current_version=${BASH_REMATCH[2]}

        if [ "X$curr_source_code" != "X" ] && \
           [ "$curr_source_code" != "$source_code" ];then
            patches_to_apply+=( $latest_patch)
            latest_version=1
        fi

        curr_source_code=$source_code

        if [ "$latest_version" -lt "$current_version" ];then
            latest_version=$current_version
            latest_patch=$PATCH
        fi
    fi
done

patches_to_apply+=( $latest_patch )

for PATCH_APPLY in "${patches_to_apply[@]}";do
    >&2 echo "Applying == $PATCH_APPLY =="
    patch -d/ -b -V numbered -p0 < "$PATCH_DIR/$PATCH_APPLY"
done
