#!/bin/sh

ps -T -p `ps -ef |grep netproto | grep -v grep | grep -vP '\/bin\/sh' | grep -v watch | awk '{ print $2 }'`
