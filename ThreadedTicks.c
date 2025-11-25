#include <threads.h>
#include <stdatomic.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "Util.h"


thrd_t threads[MAX_THREADS];
bool intialized = false;
_Atomic block *threadWork[MAX_THREADS];
atomic_uint_fast8_t ActiveCount = 0;

atomic_uint_fast64_t workingCount = 0;
atomic_bool work = false;
atomic_bool terminateThreads = false;
atomic_bool globalFlipBit = false;
int workerThread(void *arg) {
    uint_fast8_t threadID = (uint_fast8_t)(uintptr_t)arg;
    bool localWork = false;
    while (true) {
        while (!atomic_load(&work) && !atomic_load(&terminateThreads)) {
            if (localWork&&!atomic_load(&work)) {
                atomic_fetch_sub(&workingCount, 1);
                localWork = false;
            }
            thrd_yield();
        }
        if (!localWork&&atomic_load(&work)) {
            atomic_fetch_add(&workingCount, 1);
            localWork = true;
        }
        
        if ((atomic_load(&ActiveCount) == 0) || (atomic_load(&terminateThreads) && threadID >= atomic_load(&ActiveCount)-1)) {
            return 0;
        }
        
        while (atomic_load(&work)) {
            _Atomic block *b = atomic_load(&threadWork[threadID]); //main writes to and then this thread notices it and will set it to NULL afterwards so that main knows to write to it again.
            atomic_store(&threadWork[threadID], NULL);
            if (!b) {
                thrd_yield();
                continue;
            }
            computeBlock((block *)b, atomic_load(&globalFlipBit));
        }
    }
}

static inline bool insertWorkFirstAvailable(block *b) {
    for (uint_fast8_t i = 0; i < atomic_load(&ActiveCount); i++) {
        if (!atomic_load(&threadWork[i])) {
            atomic_store(&threadWork[i], (_Atomic block *)b);
            return 1;
        }
    }
    return 0;
}

void threadedTickCalc(block *list, bool flipBit) {
    atomic_store(&globalFlipBit, flipBit);
    work = true;
    block *b = list;
    while (b != NULL) {
        while (!insertWorkFirstAvailable(b)); //keep trying to put the value in until there's a free space.
        b = b->next;
    }
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
    if (!intialized) memset(threadWork, 0, sizeof(block *) * MAX_THREADS);
    if (count > MAX_THREADS) count = MAX_THREADS;
    if (ActiveCount == count) return;
    if (ActiveCount < count) createThreads(count-ActiveCount);
    if (ActiveCount > count) killThreads(ActiveCount-count);
}

static inline void killAllThreads() {
    setThreadCount(0);
}