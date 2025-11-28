#!/bin/bash

SAVESTRINGANDTICKING = $(cat ./input.txt)

echo "testing single threaded:"
time ./Main $SAVESTRINGANDTICKING 0
echo "testing double threaded, should be slower:"
time ./Main $SAVESTRINGANDTICKING 1
echo "testing triple threaded:"
time ./Main $SAVESTRINGANDTICKING 2
echo "testing quadruple threaded:"
time ./Main $SAVESTRINGANDTICKING 3
echo "testing quintuple threaded:"
time ./Main $SAVESTRINGANDTICKING 4
echo "testing sextuple threaded:"
time ./Main $SAVESTRINGANDTICKING 5
echo "testing septuple threaded:"
time ./Main $SAVESTRINGANDTICKING 6
echo "testing octuple threaded:"
time ./Main $SAVESTRINGANDTICKING 8