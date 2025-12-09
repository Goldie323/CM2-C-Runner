#ifndef BLOCKCREATION_H
#define BLOCKCREATION_H

#include <stdio.h>
#include <stdint.h>
#include "Consts.h"

// these 4 functions should never be used outside of removeConnection and addConnection. Low key should just add them to the code for add and remove connection.
void addInput(block *bDst, block *bFrom);
void addOutput(block *bSrc, block *bTo);
void removeInput(block *bDst, block *bFrom);
void removeOutput(block *bSrc, block *bTo);



static inline void removeConnection(block *bFrom, block *bTo) {
    removeOutput(bFrom, bTo);
    removeInput(bTo, bFrom);
}
static inline void addConnection(block *bFrom, block *bTo) {
    addOutput(bFrom, bTo);
    addInput(bTo, bFrom);
}
void removeBlock(block *b);
block *CreateBlock(uint_fast8_t id, long int x, long int y, long int z, uint_fast8_t owner);

#endif