#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "BlockCreation.h"
#include "SavestringParse.h"
#include "ThreadedTicks.h"

void sleepForTick(double seconds) {
    struct timespec req, rem;
    req.tv_sec = (time_t)seconds;
    req.tv_nsec = (long)((seconds - req.tv_sec) * 1e9);
    while (nanosleep(&req, &rem) == -1) {
        req = rem; // Continue sleeping for the remaining time
    }
}

block *start = NULL;
bool flipBit = 0;

#define TAKE_ARGS 1 //eventually it'll always take the command line args but for now I like it having some build in options just since I don't want to have the issue of something wrong with the command line arguments or whatever.
//also hopefully eventually it wouldn't even use that many command line args, hopefully the only arg at some point should just be a file that represents the current state of the simulation and then the program reads that and starts executing from that save point
int main(unsigned long int argc, char *argv[]) {
   
    const char *input;
    unsigned long int totalTicks;
    unsigned long int ticksPerSecond;
    uint_fast8_t threadCount;
    if (TAKE_ARGS) {
        if (!argc==5) {
            printf("Wrong amount of inputs, there should be 3 and in the order of '%s {SaveString} {TotalTicks} {TickRate} {ThreadCount}", argv[0]);
        }
        input = argv[1];
        totalTicks = atol(argv[2]);
        ticksPerSecond = atol(argv[3]);
        threadCount = atol(argv[4]);
    }
    else {
        input = "0,0,-4,0,18,;0,0,-3,0,18,;0,0,-2,0,18,;0,0,-1,0,18,;0,0,0,0,18,;0,0,1,0,18,;0,0,2,0,18,;0,0,3,0,18,;0,0,4,0,18,;0,0,5,0,18,;2,0,-4,0,16,;2,0,-3,0,16,;2,0,-2,0,16,;2,0,-1,0,16,;2,0,0,0,16,;2,0,1,0,16,;2,0,2,0,16,;2,0,3,0,16,;2,0,4,0,16,;2,0,5,0,16,;2,0,-4,0,14,;2,0,-3,0,14,;2,0,-2,0,14,;2,0,-1,0,14,;2,0,0,0,14,;2,0,1,0,14,;2,0,2,0,14,;2,0,3,0,14,;2,0,4,0,14,;2,0,5,0,14,;2,0,-4,0,12,;2,0,-3,0,12,;2,0,-2,0,12,;2,0,-1,0,12,;2,0,0,0,12,;2,0,1,0,12,;2,0,2,0,12,;2,0,3,0,12,;2,0,4,0,12,;2,0,5,0,12,;2,0,-4,0,10,;2,0,-3,0,10,;2,0,-2,0,10,;2,0,-1,0,10,;2,0,0,0,10,;2,0,1,0,10,;2,0,2,0,10,;2,0,3,0,10,;2,0,4,0,10,;2,0,5,0,10,;2,0,-4,0,8,;2,0,-3,0,8,;2,0,-2,0,8,;2,0,-1,0,8,;2,0,0,0,8,;2,0,1,0,8,;2,0,2,0,8,;2,0,3,0,8,;2,0,4,0,8,;2,0,5,0,8,;2,0,-4,0,6,;2,0,-3,0,6,;2,0,-2,0,6,;2,0,-1,0,6,;2,0,0,0,6,;2,0,1,0,6,;2,0,2,0,6,;2,0,3,0,6,;2,0,4,0,6,;2,0,5,0,6,;2,0,-4,0,4,;2,0,-3,0,4,;2,0,-2,0,4,;2,0,-1,0,4,;2,0,0,0,4,;2,0,1,0,4,;2,0,2,0,4,;2,0,3,0,4,;2,0,4,0,4,;2,0,5,0,4,;2,0,-4,0,2,;2,0,-3,0,2,;2,0,-2,0,2,;2,0,-1,0,2,;2,0,0,0,2,;2,0,1,0,2,;2,0,2,0,2,;2,0,3,0,2,;2,0,4,0,2,;2,0,5,0,2,;2,0,-4,0,0,;2,0,-3,0,0,;2,0,-2,0,0,;2,0,-1,0,0,;2,0,0,0,0,;2,0,1,0,0,;2,0,2,0,0,;2,0,3,0,0,;2,0,4,0,0,;2,0,5,0,0,;2,0,-4,0,-2,;2,0,-3,0,-2,;2,0,-2,0,-2,;2,0,-1,0,-2,;2,0,0,0,-2,;2,0,1,0,-2,;2,0,2,0,-2,;2,0,3,0,-2,;2,0,4,0,-2,;2,0,5,0,-2,;2,0,-4,0,-4,;2,0,-3,0,-4,;2,0,-2,0,-4,;2,0,-1,0,-4,;2,0,0,0,-4,;2,0,1,0,-4,;2,0,2,0,-4,;2,0,3,0,-4,;2,0,4,0,-4,;2,0,5,0,-4,;2,0,-4,0,-6,;2,0,-3,0,-6,;2,0,-2,0,-6,;2,0,-1,0,-6,;2,0,0,0,-6,;2,0,1,0,-6,;2,0,2,0,-6,;2,0,3,0,-6,;2,0,4,0,-6,;2,0,5,0,-6,;2,0,-4,0,-8,;2,0,-3,0,-8,;2,0,-2,0,-8,;2,0,-1,0,-8,;2,0,0,0,-8,;2,0,1,0,-8,;2,0,2,0,-8,;2,0,3,0,-8,;2,0,4,0,-8,;2,0,5,0,-8,;2,0,-4,0,-10,;2,0,-3,0,-10,;2,0,-2,0,-10,;2,0,-1,0,-10,;2,0,0,0,-10,;2,0,1,0,-10,;2,0,2,0,-10,;2,0,3,0,-10,;2,0,4,0,-10,;2,0,5,0,-10,;2,0,-4,0,-12,;2,0,-3,0,-12,;2,0,-2,0,-12,;2,0,-1,0,-12,;2,0,0,0,-12,;2,0,1,0,-12,;2,0,2,0,-12,;2,0,3,0,-12,;2,0,4,0,-12,;2,0,5,0,-12,;2,0,-4,0,-14,;2,0,-3,0,-14,;2,0,-2,0,-14,;2,0,-1,0,-14,;2,0,0,0,-14,;2,0,1,0,-14,;2,0,2,0,-14,;2,0,3,0,-14,;2,0,4,0,-14,;2,0,5,0,-14,;2,0,-4,0,-16,;2,0,-3,0,-16,;2,0,-2,0,-16,;2,0,-1,0,-16,;2,0,0,0,-16,;2,0,1,0,-16,;2,0,2,0,-16,;2,0,3,0,-16,;2,0,4,0,-16,;2,0,5,0,-16,;2,0,-4,0,-18,;2,0,-3,0,-18,;2,0,-2,0,-18,;2,0,-1,0,-18,;2,0,0,0,-18,;2,0,1,0,-18,;2,0,2,0,-18,;2,0,3,0,-18,;2,0,4,0,-18,;2,0,5,0,-18,;2,0,-4,0,-20,;2,0,-3,0,-20,;2,0,-2,0,-20,;2,0,-1,0,-20,;2,0,0,0,-20,;2,0,1,0,-20,;2,0,2,0,-20,;2,0,3,0,-20,;2,0,4,0,-20,;2,0,5,0,-20,?11,21;12,22;13,23;14,24;15,25;16,26;17,27;18,28;19,29;20,30;21,31;22,32;23,33;24,34;25,35;26,36;27,37;28,38;29,39;30,40;41,51;42,52;43,53;44,54;45,55;46,56;47,57;48,58;49,59;50,60;31,41;32,42;33,43;34,44;35,45;36,46;37,47;38,48;39,49;40,50;51,61;52,62;53,63;54,64;55,65;56,66;57,67;58,68;59,69;60,70;61,71;62,72;63,73;64,74;65,75;66,76;67,77;68,78;69,79;70,80;71,81;72,82;73,83;74,84;75,85;76,86;77,87;78,88;79,89;80,90;81,91;82,92;83,93;84,94;85,95;86,96;87,97;88,98;89,99;90,100;91,101;92,102;93,103;94,104;95,105;96,106;97,107;98,108;99,109;100,110;101,111;102,112;103,113;104,114;105,115;106,116;107,117;108,118;109,119;110,120;111,121;112,122;113,123;114,124;115,125;116,126;117,127;118,128;119,129;120,130;121,131;122,132;123,133;124,134;125,135;126,136;127,137;128,138;129,139;130,140;131,141;132,142;133,143;134,144;135,145;136,146;137,147;138,148;139,149;140,150;141,151;142,152;143,153;144,154;145,155;146,156;147,157;148,158;149,159;150,160;151,161;152,162;153,163;154,164;155,165;156,166;157,167;158,168;159,169;160,170;161,171;162,172;163,173;164,174;165,175;166,176;167,177;168,178;169,179;170,180;171,181;172,182;173,183;174,184;175,185;176,186;177,187;178,188;179,189;180,190;181,191;182,192;183,193;184,194;185,195;186,196;187,197;188,198;189,199;190,200;191,1;192,2;193,3;194,4;195,5;196,6;197,7;198,8;199,9;200,10;1,11;2,12;3,13;4,14;5,15;6,16;7,17;8,18;9,19;10,20??";
        //input = "0,0,0,0,0,;2,0,0,0,0,;?1,2;2,1;??";
        totalTicks = 10;
        ticksPerSecond = 1;
        threadCount = 8;
    }

    block *list = parseFull(input, 1, flipBit);
    /*
    block *list = NULL;
    bool flipBit = false;
    block *block1 = CreateBlock(0, 0, 0, 0, 200);
    block *block2 = CreateBlock(2, 0, 0, 0, 200);
    addConnection(block1, block2);
    addConnection(block2, block1);
    block1->next = block2;
    list = block1;
    block2->next = NULL;
    */
    setThreadCount(threadCount);
    double tickDuration = (ticksPerSecond > 0) ? 1.0 / ticksPerSecond : 0;
    for (unsigned long int t = 0; t < totalTicks; t++) {
        clock_t start = (ticksPerSecond > 0) ? clock() : 0;
        tick(list, flipBit);
        flipBit = !flipBit;
        
        if (ticksPerSecond > 0) {
            clock_t end = clock();
            double elapsed = (double)(end - start)/CLOCKS_PER_SEC;
            if (elapsed < tickDuration) sleepForTick(tickDuration - elapsed);
        }

        printf("\rTick %lld | ", t+1);
        block *b = list;
        unsigned long int i = 0;
        while (b != NULL) {
            printf("%dB%lu:%d ", i++, getID(b->meta), getState(b, flipBit));
            b = b->next;
        }
        fflush(stdout);
    }
    printf("\n");

    killAllThreads();
    printf("\rAll threads joined\n");

    // Free dynamic memory
    block *b = list;
    list = NULL;
    block *temp = NULL;
    while (b != NULL) {
        removeBlock(b);
        temp = b->next; //this works because removeBlock only frees the dynamic memory within the element and not including the next element, and sets the block to 'dead'(meta==255)
        free(b);
        b = temp;
    }
    printf("\rAll blocks freed                    \n");
    return 0;
}