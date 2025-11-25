#ifndef THREADTICKS_H
#define THREADTICKS_H

#include <threads.h>
#include <stdatomic.h>
#include "Consts.h"
#include "GateFuncs.h"

void tick(block *list, bool flipBit);
void setThreadCount(__uint8_t count);
static inline void killAllThreads() {
    setThreadCount(0);
}

#endif