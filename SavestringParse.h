#ifndef SAVESTRINGPARSE_H
#define SAVESTRINGPARSE_H

#include <stdbool.h>
#include <stdio.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "BlockCreation.h"

void parseBlocks(const char *input, __uint8_t owner);
void parseConnections(const char *input);
void parseFull(const char *input, __uint8_t owner);

#endif