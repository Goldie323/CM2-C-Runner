#!/bin/bash

gcc -o ./Main /*.c
./Main "$(cat ./input.txt)" 1
