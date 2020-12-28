#!/bin/sh

# generates 1GB file 'bigfile'
dd if=/dev/urandom of=bigfile count=1048576 bs=1024
