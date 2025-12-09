#include <stdbool.h>
#include "Consts.h"
#include "GateFuncs.h"

void tick(block *list, bool flipBit) {
    block *b = list;
    while (b != NULL) {
        computeBlock(b, flipBit);
        b = b->next;
    }
}

void setThreadCount(uint_fast8_t count) {
    return;
}

static inline void killAllThreads() {
    setThreadCount(0);
}