#include <threads.h>
#include <stdatomic.h>
#include "Consts.h"
#include "GateFuncs.h"


thrd_t threads[MAX_THREADS];
__uint8_t ActiveCount = 0;

atomic_uint_least64_t taskIndex;
atomic_uint_least64_t blocksRemaining;

bool terminateThreads = false;
bool terminateOneThread = false;
int workerThread(void *arg) {
    __uint8_t threadID = (__uint8_t)(uintptr_t)arg;
    while (!terminateThreads) {

        while (atomic_load(&blocksRemaining) == 0 && !(terminateOneThread || terminateThreads)) thrd_yield();
        
        if (terminateOneThread && threadID == ActiveCount - 1) {
            return 0;
        }

        unsigned long int i = atomic_fetch_add(&taskIndex, 1);
        if (i >= blockCount) break;
        computeBlock(i);
        atomic_fetch_sub(&blocksRemaining, 1);
    }
    return 0;
}

void threadedTickCalc() {
    atomic_store(&taskIndex, 0);
    atomic_store(&blocksRemaining, blockCount);

    unsigned long int i;
    while ((i = atomic_fetch_add(&taskIndex, 1)) < blockCount) {
        computeBlock(i);
        atomic_fetch_sub(&blocksRemaining, 1);
    }

    while (atomic_load(&blocksRemaining) > 0) thrd_yield();
}

void TickCalc() {
    unsigned long int taskIndex = 0;
    unsigned long int blocksRemaining = blockCount;

    unsigned long int i;
    while ((i = taskIndex++) < blockCount) {
        computeBlock(i);
        blocksRemaining--;
    }

    return;
}

void tick() {
    // add before tick operations like editing blocks and such
    if (ActiveCount > 0) {
        threadedTickCalc();
    }
    else {
        TickCalc();
    }
    bool *tmp = state;
    state = preState;
    preState = tmp;
}

void killOneThread() {
    if (ActiveCount == 0) return;
    
    terminateOneThread = true;
    
    int result;
    thrd_join(threads[ActiveCount - 1], &result);
    
    terminateOneThread = false;
    ActiveCount--;
}

void createOneThread() {
    thrd_create(&threads[ActiveCount], workerThread, (void *)(uintptr_t)ActiveCount);
    ActiveCount++;
}

void setThreadCount(__uint8_t count) {
    if (count < 0) count = 0;
    if (count > MAX_THREADS) count = MAX_THREADS;

    if (ActiveCount == count) return;

    while (ActiveCount < count) {
        createOneThread();
    }

    while (ActiveCount > count) {
        killOneThread();
    }
    return;
}

void killAllThreads() {
    terminateThreads = true;
    atomic_store(&blocksRemaining, 0); // Reset to 0 to allow threads to exit

    for (unsigned long int i = 0; i < ActiveCount; i++) {
        thrd_join(threads[i], NULL);
    }
    ActiveCount = 0;
}