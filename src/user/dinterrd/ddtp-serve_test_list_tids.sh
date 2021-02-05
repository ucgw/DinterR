#!/bin/sh

ps -T -p `ps -ef | grep ddtp-serve | grep -v grep | grep -vP '\/bin\/sh' | grep -v watch | awk '{ print $2 }'` 2>/dev/null
