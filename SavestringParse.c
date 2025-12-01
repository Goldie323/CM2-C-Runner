#include <stdbool.h>
#include <stdio.h>
#include "Util.h"
#include "Consts.h"
#include "GateFuncs.h"
#include "BlockCreation.h"
// fix these functions, they kind of suck and are fully vibe coded and probably aren't good. Fix them later and maybe add a parseString function that does the whole thing, also add support for
block *parseBlocks(const char *ptr, uint_fast8_t owner, bool flipBit) {
    block *b = NULL;
    block *end = NULL;
    bool firstLoop = true;
    while (*ptr && *ptr != '?') {
        int idTmp, sTmp = 0;
        long int x, y, z = 0;
        unsigned int charsRead = 0;
    
        // Read basic block info safely
        printf("entering DEATH\n");
        int lifeLine;
        if (lifeLine = sscanf(ptr, "%d,%d,%ld,%ld,%ld%n", &idTmp, &sTmp, &x, &y, &z, &charsRead) < 5) printf("error: not five(%d) thingy's found at place: %s", lifeLine, ptr);
        printf("exiting DEATH\n");
        uint_fast8_t id = (uint_fast8_t)idTmp;
        bool s = (sTmp != 0);
        ptr += charsRead;
        // Create the block
        if (firstLoop) {
            b = CreateBlock(id, x, y, z, owner);
            firstLoop = false;
            end = b;
        }
        else {
            end->next = CreateBlock(id, x, y, z, owner);
            end = end->next;
        }
        if (!end) break;
        setState(end, flipBit, s);

        // Read extra data if present
        if (*ptr == ',') {
            ptr++; // skip comma
            const char *semi = strchr(ptr, ';');
            if (!semi) semi = strchr(ptr, '?');
            if (semi) {
                unsigned long int len = (unsigned long int)(semi - ptr);
                char *buffer = smalloc(len + 1); // Dynamic buffer allocation
                memcpy(buffer, ptr, len);
                buffer[len] = '\0';

                switch (id) {
                    case FLIPFLOP: { // Flip Flop Block
                        flipFlopBlock *f = (flipFlopBlock *)end;
                        f->PrevXor = 1;
                        break;
                    }
                    case LED: { // LED Block
                        ledBlock *l = (ledBlock *)end;
                        l->RedAmount = 255;
                        l->GreenAmount = 255;
                        l->BlueAmount = 255;
                        l->OpacityOn = 100;
                        l->OpacityOff = 5;
                        l->analog = false;

                        unsigned long int values[6];
                        unsigned long int count = sscanf(buffer, "%d+%d+%d+%d+%d+%d",
                                           &values[0], &values[1], &values[2],
                                           &values[3], &values[4], &values[5]);
                        if (count >= 1) l->RedAmount = values[0];
                        if (count >= 2) l->GreenAmount = values[1];
                        if (count >= 3) l->BlueAmount = values[2];
                        if (count >= 4) l->OpacityOn = values[3];
                        if (count >= 5) l->OpacityOff = values[4];
                        if (count >= 6) l->analog = (values[5] != 0);
                        break;
                    }
                    case SOUND: { // Sound Block
                        soundBlock *s = (soundBlock *)end;
                        s->Frequency = 44000;
                        s->Instrument = 0;

                        unsigned long int values[2];
                        unsigned long int count = sscanf(buffer, "%d+%d", &values[0], &values[1]);
                        if (count >= 1) s->Frequency = values[0];
                        if (count >= 2) s->Instrument = values[1];
                        break;
                    }
                    case RANDOM: { // Random Block
                        randomBlock *r = (randomBlock *)end;
                        r->Probability = 50;
                        unsigned long int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            r->Probability = value;
                        break;
                    }
                    case TEXT: { // Text Block
                        charBlock *c = (charBlock *)end;
                        c->Character = 'A';
                        unsigned long int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            c->Character = (char)value;
                        break;
                    }
                    case TILE: { // Tile Block
                        tileBlock *t = (tileBlock *)end;
                        t->RedAmount = 255;
                        t->GreenAmount = 255;
                        t->BlueAmount = 255;
                        t->Material = 0;
                        t->Collision = false;

                        unsigned long int values[5];
                        unsigned long int count = sscanf(buffer, "%d+%d+%d+%d+%d",
                                           &values[0], &values[1], &values[2],
                                           &values[3], &values[4]);
                        if (count >= 1) t->RedAmount = values[0];
                        if (count >= 2) t->GreenAmount = values[1];
                        if (count >= 3) t->BlueAmount = values[2];
                        if (count >= 4) t->Material = values[3];
                        if (count >= 5) t->Collision = (values[4] != 0);
                        break;
                    }
                    case DELAY: { // Delay Block
                        delayBlock *d = (delayBlock *)end;
                        d->DelayTime = 1;
                        unsigned long int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            d->DelayTime = value;
                        break;
                    }
                    case ANTENNA: { // Antenna Block
                        antennaBlock *a = (antennaBlock *)end;
                        a->Signal = 0;
                        a->Global = false;

                        unsigned long int signal, global;
                        unsigned long int count = sscanf(buffer, "%d+%d", &signal, &global);
                        if (count >= 1) a->Signal = signal;
                        if (count >= 2) a->Global = (global != 0);
                        break;
                    }
                    case LEDMIXER: { // LED Mixer Block
                        ledMixerBlock *l = (ledMixerBlock *)end;
                        l->Additive = true;
                        unsigned long int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            l->Additive = (value != 0);
                        break;
                    }
                    default:
                        break;
                }
                free(buffer);
                ptr = semi;
            }
        }
        
        if (*ptr == ';') ptr++;
    }
    return b;
}

void parseConnections(block *b, const char *input) {
    const char *ptr = input;
    printf("%lu\n", (unsigned long int)b);
    block *loop = b;
    unsigned long int blockCap = 32768;
    unsigned long int blockSize = 0;
    block **blockDict = smalloc(sizeof(block *) * blockCap);
    while (loop!=NULL) {
        if (blockSize >= blockCap) {
            blockCap *= 2;
            blockDict = srealloc(blockDict, sizeof(block *) * blockCap);
        }
        printf("BLOCK SIZE: %lu\n", blockSize);
        blockDict[blockSize++] = loop;
        printf("blockCap: %lu; blockDict[%lu]: %lu\n", blockCap, blockSize, (unsigned long int)loop);
        loop = loop->next;
    }
    while (*ptr && *ptr != '?') {
        unsigned long int from, to;
        unsigned long int charsRead = 0;
        
        if (sscanf(ptr, "%lu,%lu%n", &from, &to, &charsRead) < 2) break;
        ptr += charsRead;
        if (*ptr == ';') ptr++;

        from--;
        to--;

        block *src = blockDict[from];
        block *dst = blockDict[to];

        if (!src || !dst) {
            fprintf(stderr, "Warning: Null block in connection %d->%d, skipping\n", from, to);
            continue;
        }

        printf("src: %lu\n", (unsigned long int)src);
        printf("dst: %lu\n", (unsigned long int)dst);

        addConnection(src, dst);
    }
    free(blockDict);
}

static inline block *parseFull(const char *input, uint_fast8_t owner, bool flipBit) {
    const char *q1 = strchr(input, '?');
    block *b = NULL;
    if (q1) {
        b = parseBlocks(input, owner, flipBit);
        if (!b) return NULL;
        parseConnections(b, q1+1);
    }
    return b;
}