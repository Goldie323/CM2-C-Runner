#include <stdio.h>
#include "Util.h"
#include "Consts.h"
#include "GateFuncs.h"

void addInput(block *bDst, block *bFrom) {
    if (!bDst) return;
    if (bDst->inputsSize == 0) {
        bDst->inputsSize = 4;
        bDst->inputs = smalloc(sizeof(block *) * bDst->inputsSize);
    } else if (bDst->inputCount >= bDst->inputsSize) {
        bDst->inputsSize *= 2;
        bDst->inputs = srealloc(bDst->inputs, sizeof(block *) * bDst->inputsSize);
    }
    bDst->inputs[bDst->inputCount++] = bFrom;
}

void addOutput(block *bSrc, block *bTo) {
    if (!bSrc) return;
    if (bSrc->outputsSize == 0) {
        bSrc->outputsSize = 4;
        bSrc->outputs = smalloc(sizeof(block *) * bSrc->outputsSize);
    } else if (bSrc->outputCount >= bSrc->outputsSize) {
        bSrc->outputsSize *= 2;
        bSrc->outputs = srealloc(bSrc->outputs, sizeof(block *) * bSrc->outputsSize);
    }
    bSrc->outputs[bSrc->outputCount++] = bTo;
}

void removeInput(block *bDst, block *bFrom) {
    if (!bDst) return;
    if (!bFrom) return;

    unsigned long int i = 0;
    while (i < bDst->inputCount && bDst->inputs[i] != bFrom) i++;
    if (i < bDst->inputCount) bDst->inputs[i] = bDst->inputs[--bDst->inputCount];
    else return;
    bDst->inputCount--;
}

void removeOutput(block *bSrc, block *bTo) {
    if (!bSrc) return;
    if (!bTo) return;

    unsigned long int i = 0;
    while (i < bSrc->outputCount && bSrc->outputs[i] != bTo) i++;
    if (i < bSrc->outputCount) bSrc->outputs[i] = bSrc->outputs[--bSrc->outputCount];
    else return;
}

static inline void removeConnection(block *bFrom, block *bTo) {
    removeOutput(bFrom, bTo);
    removeInput(bTo, bFrom);
}

static inline void addConnection(block *bFrom, block *bTo) {
    addOutput(bFrom, bTo);
    addInput(bTo, bFrom);
}

void removeBlock(block *b) {
    if (!b) return;

    for (unsigned long int i = 0; i < b->inputCount; i++) removeOutput(b->inputs[i], b);
    for (unsigned long int i = 0; i < b->outputCount; i++) removeInput(b->outputs[i], b);

    free(b->inputs);
    free(b->outputs);
    b->meta = 255; // 255 marks dead, after all changes are applied it'll do one loop to remove and merge the list to remove the dead blocks.
}

size_t extraDataSize(__uint8_t id) {
    switch (id) {
        case 5: return sizeof(flipFlopBlock);
        case 6: return sizeof(ledBlock);
        case 7: return sizeof(soundBlock); 
        case 12: return sizeof(randomBlock);
        case 13: return sizeof(charBlock);
        case 14: return sizeof(tileBlock) ;
        case 16: return sizeof(delayBlock);
        case 17: return sizeof(antennaBlock);
        case 19: return sizeof(ledMixerBlock);
        default: return sizeof(block);
    }
}

block *CreateBlock(__uint8_t id, long int x, long int y, long int z, __uint8_t owner) {
    block *b = smalloc(extraDataSize(id));
    memset(b, 0, ExtraDataSize(id));
    
    setID(&b->meta, id);
    b->x = x;
    b->y = y;
    b->z = z;
    b->inputs = NULL;
    b->outputs = NULL;
    b->inputsSize = 0;
    b->outputsSize = 0;
    b->inputCount = 0;
    b->outputCount = 0;
    b->next = NULL;
    b->OwnerID = owner;

    return b;
}