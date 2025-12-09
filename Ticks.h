#ifndef TICKS_H
#define TICKS_H

//one header file for both single and multithreaded ticking. Thread related functions are still here but just return and do nothing.
#include <stdbool.h>
#include <stdint.h>
#include "Consts.h"

void tick(block *list, bool flipBit);
void setThreadCount(__uint8_t count);
static inline void killAllThreads() {
    setThreadCount(0);
}

#endif