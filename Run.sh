#!/bin/bash

gcc -o ./Main ./Main.c
./Main $(cat ./test_input.txt)