#ifndef GateFuncs_h
#define GateFuncs_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Consts.h"
#include "Util.h"

static inline uint_least8_t getID(uint_least8_t meta) {
    return meta & ID_MASK;
};

static inline void setID(uint_least8_t *meta, uint_least8_t id) {
    *meta = (*meta & ~ID_MASK) | (id & ID_MASK);
};

void setPrestate(block *b, bool flipBit, bool value);
static inline bool getState(block *b, int flipBit) {
    if (!flipBit) return getBool0(b->meta);
    else return getBool1(b->meta);
}
static inline void setPrestate(block *b, bool flipBit, bool value) {
    if (flipBit) setBool0(&b->meta, value);
    else setBool1(&b->meta, value);
}
//NEVER USE DURING THE TICK, THIS BREAKS THREAD SAFETY IF USED WITHIN THREADED CODE. This is for between ticks.
static inline void setState(block *b, bool flipBit, bool value) {
    if (!flipBit) setBool0(&b->meta, value);
    else setBool1(&b->meta, value);
}
void computeBlock(block* b, bool flipBit);

#endif