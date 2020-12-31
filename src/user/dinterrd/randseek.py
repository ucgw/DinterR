#!/usr/bin/env python
import random
import os

FILE_TO_TEST = "bigfile"

randpos1  = random.randint(10,1240900);
randpos2  = random.randint(10,129);
randpos3  = random.randint(10,1024);
randpos4  = random.randint(10,90987);
randpos5  = random.randint(10,14971);
randpos6  = random.randint(10,20);
randpos7  = random.randint(10,572120);
randpos8  = random.randint(10,3711);
randpos9  = random.randint(10,20);
randpos10 = random.randint(10,448711);
randpos11 = random.randint(10,17690346);

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos3))
    fh.seek(randpos3, 0)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from end): {0}".format(-randpos1))
    fh.seek(-randpos1, 2)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos2))
    fh.seek(randpos2, 0)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from end): {0}".format(-randpos4))
    fh.seek(-randpos4, 2)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos5))
    fh.seek(randpos5, 0)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos7))
    fh.seek(randpos7, 0)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos8))
    fh.seek(randpos8, 0)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos9))
    fh.seek(randpos9, 0)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos10))
    fh.seek(randpos10, 0)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

with open(FILE_TO_TEST, 'r') as fh:
    print("Random Pos (from start): {0}".format(randpos11))
    fh.seek(randpos11, 0)
    print(fh.tell())
    #print(fh.readline().decode('utf-8'))

print("PID: {0}".format(os.getpid()))
