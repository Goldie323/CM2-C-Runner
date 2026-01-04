#ifndef TICKS_H
#define TICKS_H

//one header file for both single and multithreaded ticking. Thread related functions are still here but just return and do nothing.
#include <stdbool.h>
#include <stdint.h>
#include "Consts.h"

void tick(block *list, bool flipBit);
//does nothing in single threaded compilation
void setThreadCount(uint_least8_t count);
//does nothing in single threaded compilation
static inline void killAllThreads() {
    setThreadCount(0);
}

#endif