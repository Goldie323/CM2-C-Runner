#!/bin/bash

gcc -o ./Main ./Main.c ./GateFuncs.c ./BlockCreation.c ./SavestringParse.c ./ThreadedTicks.c ./Util.c -lpthread -fsanitize=address -g
#gcc -o ./Main ./Main.c ./GateFuncs.c ./BlockCreation.c ./ThreadedTicks.c ./Util.c -lpthread -fsanitize=address -g
./Main $(cat ./input.txt) 1