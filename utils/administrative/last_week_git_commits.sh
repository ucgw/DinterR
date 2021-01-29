#!/bin/sh

if [ "X$1" != "X" ];then
    git log -U --since=7.days
else
    git log --since=7.days
fi
