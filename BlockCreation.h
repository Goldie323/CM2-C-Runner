#ifndef BLOCKCREATION_H
#define BLOCKCREATION_H

#include <stdio.h>
#include "Consts.h"
#include "GateFuncs.h"

#define Check_alloc_fail(var, action)  \
if (!var) {  \
    fprintf(stderr, "Error: Failed to allocate memory for buffer\n");   \
    action;  \
}

void addInput(Block *b, unsigned long int value);
void addOutput(Block *b, unsigned long int value);
void removeInput(Block *b, unsigned long int value);
void removeOutput(Block *b, unsigned long int value);
void removeConnection(unsigned long int from, unsigned long int to);
void addConnection(unsigned long int from, unsigned long int to);
void removeBlock(unsigned long int index);
Block *CreateBlock(__uint8_t id, long int x, long int y, long int z);

#endif