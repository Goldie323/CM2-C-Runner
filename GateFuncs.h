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


static inline uint_least8_t getID(uint_least8_t meta) {
    return meta & ID_MASK;
};

static inline void setID(uint_least8_t *meta, uint_least8_t id) {
    *meta = (*meta & ~ID_MASK) | (id & ID_MASK);
};

void setPrestate(block *b, bool flipBit, bool value);
bool getState(block *b, int flipBit);
void setState(block *b, bool flipBit, bool value);
void setPrestate(block *b, bool flipBit, bool value);
void computeBlock(block* b, bool flipBit);

#endif