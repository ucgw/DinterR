#!/bin/sh
source `pwd`/dinterbuild.env 2>/dev/null || exit 1
patchfile_regex='([a-zA-Z0-9\_\-\.]+)\#([0-9]+)\.'

curr_source_code=''
latest_version=1
latest_patch=''
latest_code=''
patch_init=''
patches_to_apply=()

for PATCH in `ls $PATCH_DIR`;do
    [[ $PATCH =~ $patchfile_regex ]]
    source_code=${BASH_REMATCH[1]}
    if [ "X$source_code" != "X" ];then
        current_version=${BASH_REMATCH[2]}

        if [ "$curr_source_code" != "$source_code" ];then
            curr_source_code=$source_code
            if [ "X$latest_patch" != "X" ];then
                [[ $latest_patch =~ $patchfile_regex ]]
                latest_code=${BASH_REMATCH[1]}

                if [ "X$latest_code" == "X" ];then
                    patches_to_apply+=( $PATCH )
                    latest_version=$current_version
                else
                #elif [ "$latest_code" != "$curr_source_code" ];then
                    patches_to_apply+=( $latest_patch)
                fi
            fi

        fi

        if [ "$latest_version" -lt "$current_version" ];then
            latest_version=$current_version
            latest_patch=$PATCH
        fi
    fi
done

patches_to_apply+=( $latest_patch )

for PATCH_APPLY in "${patches_to_apply[@]}";do
    >&2 echo $PATCH_APPLY
done
