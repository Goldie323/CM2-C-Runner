#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "BlockCreation.h"
#include "SavestringParse.h"

thrd_t threads[THREAD_COUNT];
atomic_uint_least64_t taskIndex;
atomic_uint_least64_t blocksRemaining;

atomic_bool terminateThreads = false;
int workerThread(void *arg) {
    (void)arg;
    while (!atomic_load(&terminateThreads)) {

        while (atomic_load(&blocksRemaining) == 0 && !atomic_load(&terminateThreads)) thrd_yield();

        unsigned long int i = atomic_fetch_add(&taskIndex, 1);
        if (i >= BlockCount) break;
        computeBlock(i);
        atomic_fetch_sub(&blocksRemaining, 1);
    }
    return 0;
}

void tickCalc() {
    atomic_store(&taskIndex, 0);
    atomic_store(&blocksRemaining, BlockCount);

    unsigned long int i;
    while ((i = atomic_fetch_add(&taskIndex, 1)) < BlockCount) {
        computeBlock(i);
        atomic_fetch_sub(&blocksRemaining, 1);
    }

    while (atomic_load(&blocksRemaining) > 0) thrd_yield();
}

void tick() {
    // add before tick operations like editing blocks and such
    tickCalc();
    bool *tmp = state;
    state = preState;
    preState = tmp;
}

void sleepForTick(double seconds) {
    struct timespec req, rem;
    req.tv_sec = (time_t)seconds;
    req.tv_nsec = (long)((seconds - req.tv_sec) * 1e9);
    while (nanosleep(&req, &rem) == -1) {
        req = rem; // Continue sleeping for the remaining time
    }
}


int main(unsigned long int argc, char *argv[]) {

    for (unsigned long int i = 0; i < THREAD_COUNT; i++) {
        thrd_create(&threads[i], workerThread, NULL);
    }

    // Get circuit input string from command line arguments
    //const char *input = argv[1];

    //const char *input = "0,0,-4,0,18,;0,0,-3,0,18,;0,0,-2,0,18,;0,0,-1,0,18,;0,0,0,0,18,;0,0,1,0,18,;0,0,2,0,18,;0,0,3,0,18,;0,0,4,0,18,;0,0,5,0,18,;2,0,-4,0,16,;2,0,-3,0,16,;2,0,-2,0,16,;2,0,-1,0,16,;2,0,0,0,16,;2,0,1,0,16,;2,0,2,0,16,;2,0,3,0,16,;2,0,4,0,16,;2,0,5,0,16,;2,0,-4,0,14,;2,0,-3,0,14,;2,0,-2,0,14,;2,0,-1,0,14,;2,0,0,0,14,;2,0,1,0,14,;2,0,2,0,14,;2,0,3,0,14,;2,0,4,0,14,;2,0,5,0,14,;2,0,-4,0,12,;2,0,-3,0,12,;2,0,-2,0,12,;2,0,-1,0,12,;2,0,0,0,12,;2,0,1,0,12,;2,0,2,0,12,;2,0,3,0,12,;2,0,4,0,12,;2,0,5,0,12,;2,0,-4,0,10,;2,0,-3,0,10,;2,0,-2,0,10,;2,0,-1,0,10,;2,0,0,0,10,;2,0,1,0,10,;2,0,2,0,10,;2,0,3,0,10,;2,0,4,0,10,;2,0,5,0,10,;2,0,-4,0,8,;2,0,-3,0,8,;2,0,-2,0,8,;2,0,-1,0,8,;2,0,0,0,8,;2,0,1,0,8,;2,0,2,0,8,;2,0,3,0,8,;2,0,4,0,8,;2,0,5,0,8,;2,0,-4,0,6,;2,0,-3,0,6,;2,0,-2,0,6,;2,0,-1,0,6,;2,0,0,0,6,;2,0,1,0,6,;2,0,2,0,6,;2,0,3,0,6,;2,0,4,0,6,;2,0,5,0,6,;2,0,-4,0,4,;2,0,-3,0,4,;2,0,-2,0,4,;2,0,-1,0,4,;2,0,0,0,4,;2,0,1,0,4,;2,0,2,0,4,;2,0,3,0,4,;2,0,4,0,4,;2,0,5,0,4,;2,0,-4,0,2,;2,0,-3,0,2,;2,0,-2,0,2,;2,0,-1,0,2,;2,0,0,0,2,;2,0,1,0,2,;2,0,2,0,2,;2,0,3,0,2,;2,0,4,0,2,;2,0,5,0,2,;2,0,-4,0,0,;2,0,-3,0,0,;2,0,-2,0,0,;2,0,-1,0,0,;2,0,0,0,0,;2,0,1,0,0,;2,0,2,0,0,;2,0,3,0,0,;2,0,4,0,0,;2,0,5,0,0,;2,0,-4,0,-2,;2,0,-3,0,-2,;2,0,-2,0,-2,;2,0,-1,0,-2,;2,0,0,0,-2,;2,0,1,0,-2,;2,0,2,0,-2,;2,0,3,0,-2,;2,0,4,0,-2,;2,0,5,0,-2,;2,0,-4,0,-4,;2,0,-3,0,-4,;2,0,-2,0,-4,;2,0,-1,0,-4,;2,0,0,0,-4,;2,0,1,0,-4,;2,0,2,0,-4,;2,0,3,0,-4,;2,0,4,0,-4,;2,0,5,0,-4,;2,0,-4,0,-6,;2,0,-3,0,-6,;2,0,-2,0,-6,;2,0,-1,0,-6,;2,0,0,0,-6,;2,0,1,0,-6,;2,0,2,0,-6,;2,0,3,0,-6,;2,0,4,0,-6,;2,0,5,0,-6,;2,0,-4,0,-8,;2,0,-3,0,-8,;2,0,-2,0,-8,;2,0,-1,0,-8,;2,0,0,0,-8,;2,0,1,0,-8,;2,0,2,0,-8,;2,0,3,0,-8,;2,0,4,0,-8,;2,0,5,0,-8,;2,0,-4,0,-10,;2,0,-3,0,-10,;2,0,-2,0,-10,;2,0,-1,0,-10,;2,0,0,0,-10,;2,0,1,0,-10,;2,0,2,0,-10,;2,0,3,0,-10,;2,0,4,0,-10,;2,0,5,0,-10,;2,0,-4,0,-12,;2,0,-3,0,-12,;2,0,-2,0,-12,;2,0,-1,0,-12,;2,0,0,0,-12,;2,0,1,0,-12,;2,0,2,0,-12,;2,0,3,0,-12,;2,0,4,0,-12,;2,0,5,0,-12,;2,0,-4,0,-14,;2,0,-3,0,-14,;2,0,-2,0,-14,;2,0,-1,0,-14,;2,0,0,0,-14,;2,0,1,0,-14,;2,0,2,0,-14,;2,0,3,0,-14,;2,0,4,0,-14,;2,0,5,0,-14,;2,0,-4,0,-16,;2,0,-3,0,-16,;2,0,-2,0,-16,;2,0,-1,0,-16,;2,0,0,0,-16,;2,0,1,0,-16,;2,0,2,0,-16,;2,0,3,0,-16,;2,0,4,0,-16,;2,0,5,0,-16,;2,0,-4,0,-18,;2,0,-3,0,-18,;2,0,-2,0,-18,;2,0,-1,0,-18,;2,0,0,0,-18,;2,0,1,0,-18,;2,0,2,0,-18,;2,0,3,0,-18,;2,0,4,0,-18,;2,0,5,0,-18,;2,0,-4,0,-20,;2,0,-3,0,-20,;2,0,-2,0,-20,;2,0,-1,0,-20,;2,0,0,0,-20,;2,0,1,0,-20,;2,0,2,0,-20,;2,0,3,0,-20,;2,0,4,0,-20,;2,0,5,0,-20,?11,21;12,22;13,23;14,24;15,25;16,26;17,27;18,28;19,29;20,30;21,31;22,32;23,33;24,34;25,35;26,36;27,37;28,38;29,39;30,40;41,51;42,52;43,53;44,54;45,55;46,56;47,57;48,58;49,59;50,60;31,41;32,42;33,43;34,44;35,45;36,46;37,47;38,48;39,49;40,50;51,61;52,62;53,63;54,64;55,65;56,66;57,67;58,68;59,69;60,70;61,71;62,72;63,73;64,74;65,75;66,76;67,77;68,78;69,79;70,80;71,81;72,82;73,83;74,84;75,85;76,86;77,87;78,88;79,89;80,90;81,91;82,92;83,93;84,94;85,95;86,96;87,97;88,98;89,99;90,100;91,101;92,102;93,103;94,104;95,105;96,106;97,107;98,108;99,109;100,110;101,111;102,112;103,113;104,114;105,115;106,116;107,117;108,118;109,119;110,120;111,121;112,122;113,123;114,124;115,125;116,126;117,127;118,128;119,129;120,130;121,131;122,132;123,133;124,134;125,135;126,136;127,137;128,138;129,139;130,140;131,141;132,142;133,143;134,144;135,145;136,146;137,147;138,148;139,149;140,150;141,151;142,152;143,153;144,154;145,155;146,156;147,157;148,158;149,159;150,160;151,161;152,162;153,163;154,164;155,165;156,166;157,167;158,168;159,169;160,170;161,171;162,172;163,173;164,174;165,175;166,176;167,177;168,178;169,179;170,180;171,181;172,182;173,183;174,184;175,185;176,186;177,187;178,188;179,189;180,190;181,191;182,192;183,193;184,194;185,195;186,196;187,197;188,198;189,199;190,200;191,1;192,2;193,3;194,4;195,5;196,6;197,7;198,8;199,9;200,10;1,11;2,12;3,13;4,14;5,15;6,16;7,17;8,18;9,19;10,20??";

    const char *input = "0,0,0,0,0,;2,0,0,0,0,;?1,2;2,1;??";

    const char *q1 = strchr(input, '?');
    if (q1) {
        size_t blockDataLen = q1 - input;
        char *blockData = malloc(blockDataLen + 1);
        Check_alloc_fail(blockData, return 1)
        strncpy(blockData, input, blockDataLen);
        blockData[blockDataLen] = '\0';
        parseBlocks(blockData, "Goldie323");
        free(blockData);

        parseConnections(q1+1);
    }
    
    // Get simulation parameters from command line arguments
    //unsigned long int totalTicks = atol(argv[2]);
    //unsigned long int ticksPerSecond = atol(argv[3]);
    

    unsigned long int totalTicks = 10;
    unsigned long int ticksPerSecond = 1;

    if (totalTicks <= 0) {
        fprintf(stderr, "Error: total ticks must be positive\n");
        return 1;
    }
    
    double tickDuration = (ticksPerSecond > 0) ? 1.0 / ticksPerSecond : 0;
    
    for (unsigned long int t = 0; t < totalTicks; t++) {
        clock_t start = (ticksPerSecond > 0) ? clock() : 0;
        tick();
        
        if (ticksPerSecond > 0) {
            clock_t end = clock();
            double elapsed = (double)(end - start)/CLOCKS_PER_SEC;
            if (elapsed < tickDuration) sleepForTick(tickDuration - elapsed);
        }

        // Prunsigned long int tick and block states on the same line, updating dynamically
        printf("\rTick %lld | ", t+1);
        for (unsigned long int i = 0; i < BlockCount; i++) {
            printf("%dB%lld:%d ", blocks[i]->ID, i, state[i]);
        }
        fflush(stdout);
    }
    printf("\n"); // Add newline after all ticks are done

    // Signal threads to terminate
    atomic_store(&terminateThreads, true);
    atomic_store(&blocksRemaining, 0); // Reset to 0 to allow threads to exit

    // Join threads
    for (unsigned long int i = 0; i < THREAD_COUNT; i++) {
        printf("\rJoining threads %ld/%ld", i+1, THREAD_COUNT);
        fflush(stdout);
        thrd_join(threads[i], NULL);
    }
    printf("\rAll threads joined                    \n");

    // Free dynamic memory
    for (unsigned long int i = 0; i < BlockCount; i++) {
        printf("\rFreeing blocks %d/%d", i+1, BlockCount);
        fflush(stdout);
        free(blocks[i]->inputs);
        free(blocks[i]->outputs);
        free(blocks[i]);
    }
    printf("\rAll blocks freed                    \n");
    return 0;
}