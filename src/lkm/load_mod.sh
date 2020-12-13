#!/bin/sh
modinfo DinterR_mod.ko
if [ "$?" == "0" ];then
    # sudo insmod DinterR_mod.ko
    sudo insmod DinterR_mod.ko dinterr_filename="/home/dinter/csce499/DinterR/src/lkm/Makefile"
fi
