#include <string.h>
#include <stdlib.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "Util.h"

void addInput(block *BlockArray, uint_least64_t bDst, uint_least64_t bFrom) {
    if (!bDst) return;
    if (BlockArray[bDst].InputsSize == 0) {
        BlockArray[bDst].InputsSize = 4;
        BlockArray[bDst].Inputs = smalloc(sizeof(uint_least64_t *) * BlockArray[bDst].InputsSize);
    } else if (BlockArray[bDst].InputCount >= BlockArray[bDst].InputsSize) {
        BlockArray[bDst].InputsSize *= 2;
        BlockArray[bDst].Inputs = srealloc(BlockArray[bDst].Inputs, sizeof(uint_least64_t *) * BlockArray[bDst].InputsSize);
    }
    BlockArray[bDst].Inputs[BlockArray[bDst].InputCount++] = bFrom;
}

void addOutput(block *BlockArray, uint_least64_t bSrc, uint_least64_t bTo) {
    if (!bSrc) return;
    if (BlockArray[bSrc].OutputsSize == 0) {
        BlockArray[bSrc].OutputsSize = 4;
        BlockArray[bSrc].Outputs = smalloc(sizeof(uint_least64_t *) * BlockArray[bSrc].OutputsSize);
    } else if (BlockArray[bSrc].OutputCount >= BlockArray[bSrc].OutputsSize) {
        BlockArray[bSrc].OutputsSize *= 2;
        BlockArray[bSrc].Outputs = srealloc(BlockArray[bSrc].Outputs, sizeof(uint_least64_t *) * BlockArray[bSrc].OutputsSize);
    }
    BlockArray[bSrc].Outputs[BlockArray[bSrc].OutputCount++] = bTo;
}

void removeInput(block *BlockArray, uint_least64_t bDst, uint_least64_t bFrom) {
    if (!bDst) return;
    if (!bFrom) return;

    unsigned long int i = 0;
    while (i < BlockArray[bDst].InputCount && BlockArray[bDst].Inputs[i] != bFrom) i++;
    if (i < BlockArray[bDst].InputCount) BlockArray[bDst].Inputs[i] = BlockArray[bDst].Inputs[--BlockArray[bDst].InputCount];
    else return;
}

void removeOutput(block *BlockArray, uint_least64_t bSrc, uint_least64_t bTo) {
    if (!bSrc) return;
    if (!bTo) return;

    unsigned long int i = 0;
    while (i < BlockArray[bSrc].OutputCount && BlockArray[bSrc].Outputs[i] != bTo) i++;
    if (i < BlockArray[bSrc].OutputCount) BlockArray[bSrc].Outputs[i] = BlockArray[bSrc].Outputs[--BlockArray[bSrc].OutputCount];
    else return;
}

static inline void removeConnection(block *BlockArray, uint_least64_t *bFrom, uint_least64_t *bTo) {
    removeOutput(BlockArray, bFrom, bTo);
    removeInput(BlockArray, bTo, bFrom);
}

static inline void addConnection(block *BlockArray, uint_least64_t *bFrom, uint_least64_t *bTo) {
    addOutput(BlockArray, bFrom, bTo);
    addInput(BlockArray, bTo, bFrom);
}

void removeBlock(block *BlockArray, uint_least64_t b) {
    if (!b) return;

    for (unsigned long int i = 0; i < BlockArray[b].InputCount; i++) removeOutput(BlockArray, BlockArray[b].Inputs[i], b);
    for (unsigned long int i = 0; i < BlockArray[b].OutputCount; i++) removeInput(BlockArray, BlockArray[b].Outputs[i], b);

    free(BlockArray[b].Inputs);
    free(BlockArray[b].Outputs);
    setDead(&BlockArray[b].Meta);
}


void CreateBlock(block *Location, uint_fast8_t id, long int x, long int y, long int z, uint_fast8_t owner) {
    memset(Location, 0, sizeof(block));
    
    setID(&Location->Meta, id);
    Location->x = x;
    Location->y = y;
    Location->z = z;
    Location->Inputs = NULL;
    Location->Outputs = NULL;
    Location->InputsSize = 0;
    Location->OutputsSize = 0;
    Location->InputCount = 0;
    Location->OutputCount = 0;
    Location->OwnerID = owner;
}