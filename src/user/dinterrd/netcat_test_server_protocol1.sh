#!/bin/sh
port=""

if [ -z $1 ];then
    echo "Usage: $0 <port>"
    exit 1;
fi

port=$1

# 0x01 should be the only type that is initially
# allowed on a client session
# the remaining ones should fail validation and the
# the dinterrd service should tranistion to the terminal
# state.
#echo -n -e "\x01\x10\x20\x40" | nc localhost $port
echo -n -e "\x10" | nc localhost $port
echo -n -e "\x20" | nc localhost $port
echo -n -e "\x40" | nc localhost $port
echo -n -e "\xa1" | nc localhost $port
