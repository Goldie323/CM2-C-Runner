#ifndef SAVESTRINGPARSE_H
#define SAVESTRINGPARSE_H

#include <stdbool.h>
#include <stdio.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "BlockCreation.h"

void parseBlocks(const char *input, const char *username);
void parseConnections(const char *input);

#endif