#include <threads.h>
#include <stdatomic.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "Util.h"


thrd_t threads[MAX_THREADS];
uint_fast8_t ActiveCount = 0;

atomic_uint_fast64_t taskIndex;
volatile bool working[MAX_THREADS];
volatile bool work = false;
volatile bool terminateAllThreads = false;
volatile bool terminateThreads = false;
int workerThread(void *arg) {
    uint_fast8_t threadID = (uint_fast8_t)(uintptr_t)arg;
    while (!terminateAllThreads) {
        while (!work && !(terminateThreads || terminateAllThreads)) {
            working[threadID] = false;
            thrd_yield();
        }
        
        if (terminateThreads && threadID >= ActiveCount - 1) {
            return 0;
        }

        if (terminateAllThreads) {
            return 0;
        }

        if (!work) {
            break;
        }
        working[threadID] = true;
        unsigned long int i = atomic_fetch_add(&taskIndex, 1);
        if (i >= blockCount) break;
        computeBlock(i);
    }
    return 0;
}

bool CheckWorking() {
   for (uint_fast8_t i = 0; i < ActiveCount; i++) {
      if (working[i]) return true;
   }
   return false;
}

void threadedTickCalc() {
    atomic_store(&taskIndex, 0);
    work = true;

    unsigned long int i;
    while ((i = atomic_fetch_add(&taskIndex, 1)) < blockCount) {
        computeBlock(i);
    }
    while (CheckWorking()) thrd_yield();
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
    if (count > MAX_THREADS) count = MAX_THREADS;

    if (ActiveCount == count) return;

    if (ActiveCount < count) {
        createThreads(count-ActiveCount);
    }

    if (ActiveCount > count) {
        killThreads(ActiveCount-count);
    }
    return;
}

void killAllThreads() {
    terminateAllThreads = true;
    work = false;

    for (unsigned long int i = 0; i < ActiveCount; i++) {
        thrd_join(threads[i], NULL);
    }
    ActiveCount = 0;
}