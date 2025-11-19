#ifndef GateFuncs_h
#define GateFuncs_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Consts.h"
#include "Util.h"

extern block *blocks;
extern bool flipBit;

void computeBlock(block* b, bool flipBit);

#endif