#!/usr/bin/env python
import random
import os

randpos1 = random.randint(10,20);
randpos2 = random.randint(10,20);
randpos3 = random.randint(10,20);
randpos4 = random.randint(10,20);

with open('Makefile', 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos3))
    fh.seek(randpos3, 0)
    print(fh.tell())
    print(fh.readline().decode('utf-8'))

with open('Makefile', 'r') as fh:
    print("Random Pos (from end): {0}".format(-randpos1))
    fh.seek(-randpos1, 2)
    print(fh.tell())
    print(fh.readline().decode('utf-8'))

with open('Makefile', 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos2))
    fh.seek(randpos2, 0)
    print(fh.tell())
    print(fh.readline().decode('utf-8'))

with open('Makefile', 'r') as fh:
    print("Random Pos (from end): {0}".format(-randpos4))
    fh.seek(-randpos4, 2)
    print(fh.tell())
    print(fh.readline().decode('utf-8'))

print("PID: {0}".format(os.getpid()))