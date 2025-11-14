#include <stdio.h>
#include "Util.h"
#include "Consts.h"
#include "GateFuncs.h"

void addInput(Block *b, unsigned long int value) {
    if (!b) return;
    if (b->inputsSize == 0) {
        b->inputsSize = 4;
        b->inputs = smalloc(sizeof(unsigned long int) * b->inputsSize);
    } else if (b->inputCount >= b->inputsSize) {
        b->inputsSize *= 2;
        b->inputs = srealloc(b->inputs, sizeof(unsigned long int) * b->inputsSize);
    }
    b->inputs[b->inputCount++] = value;
}

void addOutput(Block *b, unsigned long int value) {
    if (!b) return;
    if (b->outputsSize == 0) {
        b->outputsSize = 4;
        b->outputs = smalloc(sizeof(unsigned long int) * b->outputsSize);
    } else if (b->outputCount >= b->outputsSize) {
        b->outputsSize *= 2;
        b->outputs = srealloc(b->outputs, sizeof(unsigned long int) * b->outputsSize);
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
    if (index < 0 || index >= blockCount) return;

    Block *b = blocks[index];

    for (unsigned long int i = 0; i < b->inputCount; i++) {
        unsigned long int inputIdx = b->inputs[i];
        if (inputIdx >= 0 && inputIdx < blockCount) {
            removeOutput(blocks[inputIdx], index);
        }
    }
    for (unsigned long int i = 0; i < b->outputCount; i++) {
        unsigned long int outputIdx = b->outputs[i];
        if (outputIdx >= 0 && outputIdx < blockCount) {
            removeInput(blocks[outputIdx], index);
        }
    }

    free(b->inputs);
    free(b->outputs);
    free(b);

    blockCount--;

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
    if (blockCount >= blockCapacity) {
        if (blockCapacity == 0) blockCapacity = START_BLOCKS;
        else blockCapacity *= 2;
        blocks = srealloc(blocks, blockCapacity * sizeof(Block *));
        state = srealloc(state, blockCapacity * sizeof(bool));
        /*
            faster than realloc because of the possibilty of it copying
            the data to the new allocation and the current state in preState
            doesn't matter only state matters and preState is used for
            calculations which currently aren't being done as blocks are
            added before the simulation or inbetween ticks meaning that
            doing it this way will never remove necassary data and best
            case would be check if it can grow in place and do it and 
            fall back to free and malloc but there's nothing like that
            and it's a lot to make something custom just for this since
            the custom thing would need to handle system calls and would
            likely need possibly even a custom kernel that handles it and
            that'd only be for a tiny increase in efficiency and speed in
            the case that it can grow in place.
        */
        free(preState);
        preState = smalloc(blockCapacity * sizeof(bool));
    }
        
    Block *b = smalloc(extraDataSize(id));
    
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