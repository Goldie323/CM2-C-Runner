#!/bin/bash

gcc -o ./Main ./Main.c ./GateFuncs.c ./BlockCreation.c ./SavestringParse.c ./ThreadedTicks.c -lpthread
./Main $(cat ./input.txt)