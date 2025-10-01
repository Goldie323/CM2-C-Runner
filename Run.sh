#!/bin/bash

gcc -o ./Main ./Main.c ./GateFuncs.c ./BlockCreation.c ./SavestringParse.c -lpthread
./Main $(cat ./input.txt)