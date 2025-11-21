#ifndef BLOCKCREATION_H
#define BLOCKCREATION_H

#include <stdio.h>
#include "Consts.h"
#include "GateFuncs.h"

void removeConnection(unsigned long int from, unsigned long int to);
void addConnection(unsigned long int from, unsigned long int to);
void removeBlock(unsigned long int index);
block *CreateBlock(__uint8_t id, long int x, long int y, long int z, __uint8_t owner);

#endif