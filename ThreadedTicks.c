#include <threads.h>
#include <stdatomic.h>
#include <stdio.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "Util.h"


thrd_t threads[MAX_THREADS];
bool intialized = false;
volatile block *threadWork[MAX_THREADS];
volatile bool writeTurn[MAX_THREADS]; //decides if it's the threads turn to write or Mains turn to write. Both can read all the time. This is just to avoid possible byte ordering issues.
volatile uint_fast8_t activeCount = 0;

atomic_uint_fast64_t workingCount = 0;
volatile bool work = false;
volatile bool terminateThreads = false;
volatile bool globalFlipBit = false;
int workerThread(void *arg) {
    uint_fast8_t threadID = (uint_fast8_t)(uintptr_t)arg;
    bool localWork = false;
    while (true) {
        while (!(work || terminateThreads)) {
            if (localWork&&!work) {
                atomic_fetch_sub(&workingCount, 1);
                localWork = false;
            }
            thrd_yield();
        }
        
        if (terminateThreads && ((activeCount == 0) || (threadID >= activeCount-1))) {
            return 0;
        }
        
        while (work) {
            localWork = true;
            if (writeTurn[threadID]) { thrd_yield(); continue; }
            block *b = (block *)threadWork[threadID]; //main writes to and then this thread notices it and will set it to NULL afterwards so that main knows to write to it again.
            threadWork[threadID] = NULL;
            if (!b) { thrd_yield(); continue; }
            computeBlock((block *)b, atomic_load(&globalFlipBit));
            writeTurn[threadID] = false;
        }
    }
}

static inline bool insertWorkFirstAvailable(block *b) {
    for (uint_fast8_t i = 0; i < activeCount; i++) {
        if (!writeTurn[i]) {
            threadWork[i] = b;
            return 1;
        }
    }
    return 0;
}

static inline bool allWorkNull() {
    for (uint_fast8_t i = 0; i < activeCount; i++) {
        //printf("POINTER[%lu]: %lu\n", i, (unsigned long)threadWork[i]);
        if (threadWork[i]) {
            return 0;
        }
    }
    return 1;
}

void threadedTickCalc(block *list, bool flipBit) {
    globalFlipBit = flipBit;
    block *b = list;
    for (uint_fast8_t i = 0; i < activeCount; i++) {
        if (!b) break;
        threadWork[i] = b;
        b = b->next;
    }
    atomic_store(&workingCount, activeCount);
    work = true;
    while (b) {
        while (!insertWorkFirstAvailable(b)) thrd_yield(); //keep trying to put the value in until there's a free space.
        b = b->next;
    }
    while (!allWorkNull()) thrd_yield();
    work = false;
    while (atomic_load(&workingCount));
}

void tickCalc(block *list, bool flipBit) {
    block *b = list;
    while (b != NULL) {
        computeBlock(b, flipBit);
        b = b->next;
    }
}

void tick(block *list, bool flipBit) {
    if (activeCount > 0) threadedTickCalc(list, flipBit);
    else tickCalc(list, flipBit);
}

void killThreads(uint_fast8_t num) {
    activeCount -= num;
    terminateThreads = true;
    for (uint_fast8_t i = 0; i < num; i++) {
        thrd_join(threads[activeCount+i], NULL);
    }
    terminateThreads = false;
    return;
}

void createThreads(uint_fast8_t num) {
    for (uint_fast8_t i = 0; i < num; i++) {
        thrd_create(&threads[activeCount], workerThread, (void *)(uintptr_t)activeCount);
        activeCount++;
    }
}

void setThreadCount(uint_fast8_t count) {
    if (!intialized) {
        memset(threadWork, 0, sizeof(block *) * MAX_THREADS);
        memset((bool *)writeTurn, 0, sizeof(bool) * MAX_THREADS);
    }
    if (count > MAX_THREADS) count = MAX_THREADS;
    if (activeCount == count) return;
    if (activeCount < count) createThreads(count-activeCount);
    if (activeCount > count) killThreads(activeCount-count);
}

static inline void killAllThreads() {
    setThreadCount(0);
}