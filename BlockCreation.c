#include <stdio.h>
#include "Consts.h"
#include "GateFuncs.h"

#define Check_alloc_fail(var, action)  \
if (!var) {  \
    fprintf(stderr, "Error: Failed to allocate memory for buffer\n");   \
    action;  \
}

void addInput(Block *b, unsigned long int value) {
    if (!b) return;
    if (b->inputsSize == 0) {
        b->inputsSize = 4;
        b->inputs = malloc(sizeof(unsigned long int) * b->inputsSize);
        Check_alloc_fail(b->inputs, exit(1))
    } else if (b->inputCount >= b->inputsSize) {
        b->inputsSize *= 2;
        b->inputs = realloc(b->inputs, sizeof(unsigned long int) * b->inputsSize);
        Check_alloc_fail(b->inputs, exit(1))
    }
    b->inputs[b->inputCount++] = value;
}

void addOutput(Block *b, unsigned long int value) {
    if (!b) return;
    if (b->outputsSize == 0) {
        b->outputsSize = 4;
        b->outputs = malloc(sizeof(unsigned long int) * b->outputsSize);
        Check_alloc_fail(b->outputs, exit(1))
    } else if (b->outputCount >= b->outputsSize) {
        b->outputsSize *= 2;
        b->outputs = realloc(b->outputs, sizeof(unsigned long int) * b->outputsSize);
        Check_alloc_fail(b->outputs, exit(1))
    }
    b->outputs[b->outputCount++] = value;
}

void removeInput(Block *b, unsigned long int value) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (b->inputs[i] == value) {
            for (unsigned long int j = i; j < b->inputCount - 1; j++) {
                b->inputs[j] = b->inputs[j + 1];
            }
            b->inputCount--;
            break;
        }
    }
}

void removeOutput(Block *b, unsigned long int value) {
    for (unsigned long int i = 0; i < b->outputCount; i++) {
        if (b->outputs[i] == value) {
            for (unsigned long int j = i; j < b->outputCount - 1; j++) {
                b->outputs[j] = b->outputs[j + 1];
            }
            b->outputCount--;
            break;
        }
    }
}

void removeConnection(unsigned long int from, unsigned long int to) {
    Block *src = blocks[from];
    Block *dst = blocks[to];

    removeOutput(src, to);
    removeInput(dst, from);
}

void addConnection(unsigned long int from, unsigned long int to) {
    Block *src = blocks[from];
    Block *dst = blocks[to];

    addOutput(src, to);
    addInput(dst, from);
}

void removeBlock(unsigned long int index) {
    if (index < 0 || index >= BlockCount) return;

    Block *b = blocks[index];

    for (unsigned long int i = 0; i < b->inputCount; i++) {
        unsigned long int inputIdx = b->inputs[i];
        if (inputIdx >= 0 && inputIdx < BlockCount) {
            removeOutput(blocks[inputIdx], index);
        }
    }
    for (unsigned long int i = 0; i < b->outputCount; i++) {
        unsigned long int outputIdx = b->outputs[i];
        if (outputIdx >= 0 && outputIdx < BlockCount) {
            removeInput(blocks[outputIdx], index);
        }
    }

    free(b->inputs);
    free(b->outputs);
    free(b);

    BlockCount--;

    blocks[index] = NULL;

}

size_t extraDataSize(__uint8_t id) {
    switch (id) {
        case 5: return sizeof(FlipFlopBlock);
        case 6: return sizeof(LedBlock);
        case 7: return sizeof(SoundBlock); 
        case 12: return sizeof(RandomBlock);
        case 13: return sizeof(CharBlock);
        case 14: return sizeof(TileBlock) ;
        case 16: return sizeof(DelayBlock);
        case 17: return sizeof(AntennaBlock);
        case 19: return sizeof(LedMixerBlock);
        default: return sizeof(Block);
    }
}

Block *CreateBlock(__uint8_t id, long int x, long int y, long int z, __uint8_t owner) {
    if (BlockCount >= START_BLOCKS) return NULL;

    Block *b = malloc(extraDataSize(id));
    Check_alloc_fail(b, return NULL)
    
    memset(b, 0, extraDataSize(id));
    b->ID = id;
    b->x = x;
    b->y = y;
    b->z = z;
    b->inputs = NULL;
    b->outputs = NULL;
    b->inputsSize = 0;
    b->outputsSize = 0;
    b->inputCount = 0;
    b->outputCount = 0;
    b->OwnerID = owner;

    return b;
}