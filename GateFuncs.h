#ifndef GateFuncs_h
#define GateFuncs_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Consts.h"

// Global variables
extern Block *blocks[START_BLOCKS];
extern unsigned long int BlockCount;
extern bool *state;
extern bool *preState;

// Gate functions
bool norGate(unsigned long int index);
bool andGate(unsigned long int index);
bool orGate(unsigned long int index);
bool xorGate(unsigned long int index);
bool ButtonGate(unsigned long int index);
bool FlipFlopGate(unsigned long int index);
bool ledGate(unsigned long int index);
bool soundGate(unsigned long int index);
bool conductorGate(unsigned long int index);
bool customGate(unsigned long int index);
bool nandGate(unsigned long int index);
bool xnorGate(unsigned long int index);
bool randomGate(unsigned long int index);
bool TextGate(unsigned long int index);
bool TileGate(unsigned long int index);
bool nodeGate(unsigned long int index);
bool delayBlock(unsigned long int index);
bool antennaGate(unsigned long int index);
bool conductor2Gate(unsigned long int index);
bool ledMixerGate(unsigned long int index);

void computeBlock(unsigned long int index);

#endif