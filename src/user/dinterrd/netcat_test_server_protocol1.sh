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
echo -n -e "\x01\x2f\x68\x6f\x6d\x65\x2f\x64\x69\x6e\x74\x65\x72\x2f\x63\x73\x63\x65\x34\x39\x39\x2f\x44\x69\x6e\x74\x65\x72\x52\x2f\x73\x72\x63\x2f\x75\x73\x65\x72\x2f\x64\x69\x6e\x74\x65\x72\x72\x64\x2f\x62\x69\x67\x66\x69\x6c\x65" | nc localhost $port
#echo -n -e "\x10" | nc localhost $port
#echo -n -e "\x20" | nc localhost $port
#echo -n -e "\x40" | nc localhost $port
#echo -n -e "\xa1" | nc localhost $port
