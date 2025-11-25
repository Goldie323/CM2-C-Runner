#include <threads.h>
#include <stdatomic.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "Util.h"


thrd_t threads[MAX_THREADS];
bool intialized = false;
block *threadWork[MAX_THREADS];
uint_fast8_t ActiveCount = 0;

atomic_uint_fast64_t workingCount;
volatile bool work = false;
volatile bool terminateThreads = false;
volatile bool globalFlipBit = false;
int workerThread(void *arg) {
    uint_fast8_t threadID = (uint_fast8_t)(uintptr_t)arg;
    bool localWork = false;
    while (true) {
        while (!work && !terminateThreads) {
            if (localWork&&!work) {
                atomic_fetch_sub(&workingCount, 1);
                localWork = false;
            }
            thrd_yield();
        }
        if (!localWork&&work) {
            atomic_fetch_add(&workingCount, 1);
            localWork = true;
        }
        
        if ((ActiveCount == 0) || (terminateThreads && threadID >= ActiveCount - 1)) {
            return 0;
        }
        
        while (work) {
            block *b = threadWork[threadID]; //main writes to and then this thread notices it and will set it to NULL afterwards so that main knows to write to it again.
            threadWork[threadID] = NULL;
            if (!b) thrd_yield();
            computeBlock(b, globalFlipBit);
        }
    }
}

static inline bool insertWorkFirstAvailable(block *b) {
    for (uint_fast8_t i = 0; i < ActiveCount; i++) {
        if (!threadWork[i]) {
            threadWork[i] = b;
            return 1;
        }
    }
    return 0;
}

void threadedTickCalc(block *list, bool flipBit) {
    globalFlipBit = flipBit;
    work = true;
    block *b = list;
    while (b != NULL) {
        while (!insertWorkFirstAvailable(b)); //keep trying to put the value in until there's a free space.
        b = b->next;
    }
    while (atomic_load(&workingCount));
}

void tickCalc(block *list, bool flipBit) {
    block *b = list;
    while (b != NULL) {
        computeBlock(b, flipBit);
        b = b->next;
    }
}

static inline void tick(block *list, bool flipBit) {
    if (ActiveCount > 0) threadedTickCalc(list, flipBit);
    else tickCalc(list, flipBit);
}

void killThreads(uint_fast8_t num) {
    ActiveCount -= num;
    terminateThreads = true;
    for (uint_fast8_t i = 0; i < num; i++) {
        thrd_join(threads[ActiveCount+i], NULL);
    }
    terminateThreads = false;
    return;
}

void createThreads(uint_fast8_t num) {
    for (uint_fast8_t i = 0; i < num; i++) {
        thrd_create(&threads[ActiveCount], workerThread, (void *)(uintptr_t)ActiveCount);
        ActiveCount++;
    }
}

void setThreadCount(uint_fast8_t count) {
    if (!intialized) memset(threadWork, NULL, sizeof(block *) * MAX_THREADS);
    if (count > MAX_THREADS) count = MAX_THREADS;
    if (ActiveCount == count) return;
    if (ActiveCount < count) createThreads(count-ActiveCount);
    if (ActiveCount > count) killThreads(ActiveCount-count);
}

static inline void killAllThreads() {
    setThreadCount(0);
}