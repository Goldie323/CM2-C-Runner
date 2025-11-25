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
}

static inline void setID(uint_least8_t *meta, uint_least8_t id) {
    *meta = (*meta & ~ID_MASK) | (id & ID_MASK);
}

static inline bool getBool0(uint_least8_t meta) {
    return (meta & BOOL0_MASK) != 0;
}

static inline void setBool0(uint_least8_t *meta, bool value) {
    if (value) *meta |= BOOL0_MASK;
    else *meta &= ~BOOL0_MASK;
}

static inline bool getBool1(uint_least8_t meta) {
    return (meta & BOOL1_MASK) != 0;
}

static inline void setBool1(uint_least8_t *meta, bool value) {
    if (value) *meta |= BOOL1_MASK;
    else *meta &= ~BOOL1_MASK;
}


static inline bool getState(block *b, int flipBit) {
    if (!flipBit) return getBool0(b->meta);
    else return getBool1(b->meta);
}

static inline void setPrestate(block *b, bool flipBit, bool value) {
    if (flipBit) setBool0(&b->meta, value);
    else setBool1(&b->meta, value);
}

static inline void setState(block *b, bool flipBit, bool value) {
    if (!flipBit) setBool0(&b->meta, value);
    else setBool1(&b->meta, value);
}

void computeBlock(block* b, bool flipBit);

#endif