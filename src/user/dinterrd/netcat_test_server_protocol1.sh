#!/bin/sh
port=""

if [ -z $1 ];then
    echo "Usage: $0 <port>"
    exit 1;
fi

port=$1

echo -n -e "\x01\x00\xFF" | nc localhost $port
echo -n -e "\x10\x00\x00" | nc localhost $port
echo -n -e "\x20\x10\x30" | nc localhost $port
echo -n -e "\x40\x00\xFE" | nc localhost $port
echo -n -e "\xa1\x00\xFE" | nc localhost $port
