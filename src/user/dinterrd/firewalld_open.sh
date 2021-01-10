#!/bin/sh
port=""

if [ -z $1 ];then
    echo "Usage: $0 <port>"
    exit 1;
fi

port=$1
sudo firewall-cmd --zone=public --add-port=${port}/tcp
