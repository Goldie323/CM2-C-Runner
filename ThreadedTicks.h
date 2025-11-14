#ifndef THREADTICKS_H
#define THREADTICKS_H

#include <threads.h>
#include <stdatomic.h>
#include "Consts.h"
#include "GateFuncs.h"

void tick();
void setThreadCount(__uint8_t count);
void killAllThreads();

#endif