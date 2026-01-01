#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Consts.h"
#include "Util.h"
#include "Meta.h"


bool norGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return false;
    }
    return true;
}

bool andGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (!getState(b->inputs[i], flipBit)) return false;
    }
    return true;
}

bool orGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

bool xorGate(block *b, bool flipBit) {
    bool result = false;
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) result = !result;
    }
    return result;
}

bool ButtonGate(block *b, bool flipBit) {
    return false;
}

bool FlipFlopGate(block *b, bool flipBit) {
    bool CurrXor = xorGate(b, flipBit);
    bool OutputState = getState(b, flipBit);
    if (!b->extraData.flipFlopBlock.PrevXor && CurrXor) OutputState = !OutputState;
    b->extraData.flipFlopBlock.PrevXor = CurrXor;
    return OutputState;
}

bool ledGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

bool soundGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

// this needs to do a linear search of all the blocks to find the ones next to it and do an or operation to find it's current value
bool conductorGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

bool customGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

bool nandGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (!getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

bool xnorGate(block *b, bool flipBit) {
    bool result = true;
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) result = !result;
    }
    return result;
}

bool randomGate(block *b, bool flipBit) {
    randomBlock *rb = (randomBlock *)b;
    __uint8_t Probability = rb->Probability; // 0 to 100
    if (Probability == 0) return false;
    if (Probability >= 100) return true;

    int randomNumber = rand() % 101;
    if (randomNumber < Probability) return true;
    return false;
}

bool TextGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

// kind of like button, gui might tell it to be on but inputs don't change it and so every tick just turn it off
bool TileGate(block *b, bool flipBit) {
    return false;
}

// this also requires a search, a better one but still need to search and it needs to check all it's outputs and check if they're a node and if they are, run the same logic on them. Needs recursion :sob:
bool nodeGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

// gonna need to fix this code
bool delayGate(block *b, bool flipBit) {
    return false;
}


// need to do a linear search and check for all the globals/same user and then if ANY of them are on then it'll turn on, otherwise it's based on the OR of the inputs
bool antennaGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

// another recursive one based on xyz and so I don't wanna do this yet because it's require a linear search of all the blocks and I don't wanna deal with that yes
bool conductor2Gate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

// does a logic or and the rest is handled by GUI
bool ledMixerGate(block *b, bool flipBit) {
    for (unsigned long int i = 0; i < b->inputCount; i++) {
        if (getState(b->inputs[i], flipBit)) return true;
    }
    return false;
}

void computeBlock(block *b, bool flipBit) {
    if (!b) {
        return;
    }
    bool result = false;
    switch (getID(b->meta)) {
        case NOR: 
            result = norGate(b, flipBit); break;
        case AND: 
            result = andGate(b, flipBit); break;
        case OR: 
            result = orGate(b, flipBit); break;
        case XOR: 
            result = xorGate(b, flipBit); break;
        case BUTTON: 
            result = ButtonGate(b, flipBit); break;
        case FLIPFLOP: 
            result = FlipFlopGate(b, flipBit); break;
        case LED: 
            result = ledGate(b, flipBit); break;
        case SOUND: 
            result = soundGate(b, flipBit); break;
        case CONDUCTOR: 
            result = conductorGate(b, flipBit); break;
        case CUSTOM: 
            result = customGate(b, flipBit); break;
        case NAND: 
            result = nandGate(b, flipBit); break;
        case XNOR: 
            result = xnorGate(b, flipBit); break;
        case RANDOM: 
            result = randomGate(b, flipBit); break;
        case TEXT: 
            result = TextGate(b, flipBit); break;
        case TILE: 
            result = TileGate(b, flipBit); break;
        case NODE: 
            result = nodeGate(b, flipBit); break;
        case DELAY: 
            result = delayGate(b, flipBit); break;
        case ANTENNA: 
            result = antennaGate(b, flipBit); break;
        case CONDUCTOR_V2: 
            result = conductor2Gate(b, flipBit); break;
        case LEDMIXER: 
            result = ledMixerGate(b, flipBit); break;
        default:
            return;
    }
    
    setPrestate(b, flipBit, result);
}