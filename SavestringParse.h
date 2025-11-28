#ifndef SAVESTRINGPARSE_H
#define SAVESTRINGPARSE_H

#include <stdbool.h>
#include <stdio.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "BlockCreation.h"

block *parseBlocks(const char *input, uint_fast8_t owner, bool flipBit);
void parseConnections(block *b, const char *input);

static inline block *parseFull(const char *input, uint_fast8_t owner, bool flipBit) {
    const char *q1 = strchr(input, '?');
    block *b = NULL;
    if (q1) {
        b = parseBlocks(input, owner, flipBit);
        if (!b) return NULL;
        parseConnections(b, q1+1);
    }
    return b;
}
#endif