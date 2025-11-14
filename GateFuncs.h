#ifndef GateFuncs_h
#define GateFuncs_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Consts.h"
#include "Util.h"

extern Block **blocks;
extern size_t blockCount;
extern size_t blockCapacity;
extern bool *state;
extern bool *preState;

void setBlockSize(size_t capacity);
void computeBlock(unsigned long int index);

#endif