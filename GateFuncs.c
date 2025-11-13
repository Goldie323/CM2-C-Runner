#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Consts.h"
#include "Util.h"

Block **blocks = NULL;
size_t blockCount = 0;
size_t blockCapacity = 0;

bool *state = NULL;
bool *preState = NULL;

void initializeBlocks(size_t startCap) {
    if (!blocks) blocks = smalloc(startCap);
    if (!state) state = smalloc(startCap);
    if (!preState) preState = smalloc(startCap);
    blockCapacity = startCap;
}

bool InfXor(const unsigned long int *inputs,  unsigned long int inputCount) {
    bool result = false;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) result = !result;
    }
    return result;
}

bool norGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return false;
    }
    return true;
}

bool andGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (!state[inputs[i]]) return false;
    }
    return true;
}

bool orGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

bool xorGate(unsigned long int index) {
    return InfXor(blocks[index]->inputs, blocks[index]->inputCount);
}

bool ButtonGate(unsigned long int index) {
    return false;
}

bool FlipFlopGate(unsigned long int index) {
    Block *b = blocks[index];
    FlipFlopBlock *Flipper = (FlipFlopBlock *)b;
    bool CurrXor = InfXor(b->inputs, b->inputCount);
    bool OutputState = state[index];
    if (!Flipper->PrevXor&CurrXor)  OutputState = !OutputState;
    Flipper->PrevXor = CurrXor;
    return OutputState;
}

bool ledGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

bool soundGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// this needs to do a linear search of all the blocks to find the ones next to it and do an or operation to find it's current value
bool conductorGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

bool customGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

bool nandGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (!state[inputs[i]]) return true;
    }
    return false;
}

bool xnorGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    bool result = true;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) result = !result;
    }
    return result;
}

bool randomGate(unsigned long int index) {
    Block *b = blocks[index];
    RandomBlock *rb = (RandomBlock *)b;
    __uint8_t Probability = rb->Probability; // 0 to 100
    if (Probability == 0) return false;
    if (Probability >= 100) return true;

    unsigned long int randomNumber = rand() / (RAND_MAX + 1.0) * 101;
    if (randomNumber < Probability) return true;
    return false;
}

bool TextGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// kind of like button, gui might tell it to be on but inputs don't change it and so every tick just turn it off
bool TileGate(unsigned long int index) {
    return false;
}

// this also requires a search, a better one but still need to search and it needs to check all it's outputs and check if they're a node and if they are, run the same logic on them. Needs recursion :sob:
bool nodeGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// gonna need to fix this code
bool delayBlock(unsigned long int index) {
    return false;
}


// need to do a linear search and check for all the globals/same user and then if ANY of them are on then it'll turn on, otherwise it's based on the OR of the inputs
bool antennaGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// another recursive one based on xyz and so I don't wanna do this yet because it's require a linear search of all the blocks and I don't wanna deal with that yes
bool conductor2Gate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// does a logic or and the rest is handled by GUI
bool ledMixerGate(unsigned long int index) {
    const unsigned long int *inputs = blocks[index]->inputs;
    unsigned long int inputCount = blocks[index]->inputCount;
    for (unsigned long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

void computeBlock(unsigned long int index) {
    Block *b = blocks[index];
    if (!b) {
        return;
    }
    bool result = false;
    switch (b->ID) {
        case 0: 
            result = norGate(index); break;
        case 1: 
            result = andGate(index); break;
        case 2: 
            result = orGate(index); break;
        case 3: 
            result = xorGate(index); break;
        case 4: 
            result = ButtonGate(index); break;
        case 5: 
            result = FlipFlopGate(index); break;
        case 6: 
            result = ledGate(index); break;
        case 7: 
            result = soundGate(index); break;
        case 8: 
            result = conductorGate(index); break;
        case 9: 
            result = customGate(index); break;
        case 10: 
            result = nandGate(index); break;
        case 11: 
            result = xnorGate(index); break;
        case 12: 
            result = randomGate(index); break;
        case 13: 
            result = TextGate(index); break;
        case 14: 
            result = TileGate(index); break;
        case 15: 
            result = nodeGate(index); break;
        case 16: 
            result = delayBlock(index); break;
        case 17: 
            result = antennaGate(index); break;
        case 18: 
            result = conductor2Gate(index); break;
        case 19: 
            result = ledMixerGate(index); break;
        default:
            return;
    }
    
    preState[index] = result;
}