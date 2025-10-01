#!/bin/bash

gcc -o ./Main ./Main.c ./GateFuncs.c -lpthread
./Main $(cat ./input.txt)