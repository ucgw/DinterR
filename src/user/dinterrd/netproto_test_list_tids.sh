#!/bin/sh

ps -T -p `ps -ef |grep netproto | grep -v grep | grep -vP '\/bin\/sh' | awk '{ print $2 }'`
