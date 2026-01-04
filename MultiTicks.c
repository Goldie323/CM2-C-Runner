#include <threads.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "Consts.h"
#include "GateFuncs.h"

thrd_t threads[MAX_THREADS];
bool intialized = false;
_Atomic block *threadWork[MAX_THREADS];
_Atomic bool slotFilled[MAX_THREADS];
_Atomic uint_fast8_t activeCount = 0;

atomic_uint_fast64_t workingCount = 0;
_Atomic bool work = false;
_Atomic bool terminateThreads = false;
_Atomic bool globalFlipBit = false;
int workerThread(void *arg) {
    uint_fast8_t threadID = (uint_fast8_t)(uintptr_t)arg;
    bool localWork = false;
    while (true) {
        while (!(atomic_load(&work) || atomic_load(&terminateThreads))) {
            if (localWork&&!atomic_load(&work)) {
                atomic_fetch_sub(&workingCount, 1);
                localWork = false;
            }
            thrd_yield();
        }
        
        if (atomic_load(&terminateThreads) && (atomic_load(&activeCount) == 0 || threadID >= atomic_load(&activeCount))) {
            return 0;
        }
        
        while (atomic_load(&work)) {
            localWork = true;
            if (!atomic_load(&slotFilled[threadID])) { thrd_yield(); continue; }
            block *b = (block *)atomic_load(&threadWork[threadID]);
            atomic_store(&threadWork[threadID], NULL);
            if (!b) { atomic_store(&slotFilled[threadID], false); thrd_yield(); continue; }
            computeBlock(b, atomic_load(&globalFlipBit));
            atomic_store(&slotFilled[threadID], false);
        }
    }
}

static inline bool insertWorkFirstAvailable(block *b) {
    for (uint_fast8_t i = 0; i < atomic_load(&activeCount); i++) {
        if (!atomic_load(&slotFilled[i])) {
            atomic_store(&threadWork[i], (_Atomic block*)b);
            atomic_store(&slotFilled[i], true);
            return 1;
        }
    }
    return 0;
}

static inline bool allWorkNull() {
    for (uint_fast8_t i = 0; i < atomic_load(&activeCount); i++) {
        if (atomic_load(&threadWork[i])) {
            return 0;
        }
    }
    return 1;
}

void threadedTickCalc(block *list, bool flipBit) {
    atomic_store(&globalFlipBit, flipBit);
    block *b = list;
    for (uint_fast8_t i = 0; i < atomic_load(&activeCount); i++) {
        if (!b) break;
        atomic_store(&threadWork[i], (_Atomic block*)b);
        atomic_store(&slotFilled[i], true);
        b = b->next;
    }
    atomic_store(&workingCount, atomic_load(&activeCount));
    atomic_store(&work, true);
    while (b) {
        while (!insertWorkFirstAvailable(b)) thrd_yield(); //keep trying to put the value in until there's a free space.
        b = b->next;
    }
    while (!allWorkNull()) thrd_yield();
    atomic_store(&work, false);
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
    if (atomic_load(&activeCount) > 0) threadedTickCalc(list, flipBit);
    else tickCalc(list, flipBit);
}

void killThreads(uint_fast8_t num) {
    atomic_fetch_sub(&activeCount, num);
    atomic_store(&terminateThreads, true);
    for (uint_fast8_t i = 0; i < num; i++) {
        thrd_join(threads[atomic_load(&activeCount)+i], NULL);
    }
    atomic_store(&terminateThreads, false);
    return;
}

void createThreads(uint_fast8_t num) {
    for (uint_fast8_t i = 0; i < num; i++) {
        thrd_create(&threads[atomic_load(&activeCount)], workerThread, (void *)(uintptr_t)atomic_load(&activeCount));
        atomic_fetch_add(&activeCount, 1);
    }
}

void setThreadCount(uint_fast8_t count) {
    if (!intialized) {
        for (size_t i = 0; i < MAX_THREADS; ++i) {
            atomic_store(&threadWork[i], NULL);
            atomic_store(&slotFilled[i], false);
        }
        intialized = true;
    }
    if (count > MAX_THREADS) count = MAX_THREADS;
    if (atomic_load(&activeCount) == count) return;
    if (atomic_load(&activeCount) < count) createThreads(count-atomic_load(&activeCount));
    if (atomic_load(&activeCount) > count) killThreads(atomic_load(&activeCount)-count);
}

static inline void killAllThreads() {
    setThreadCount(0);
}