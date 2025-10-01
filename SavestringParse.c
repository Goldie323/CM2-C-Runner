#include <stdbool.h>
#include <stdio.h>
#include "Consts.h"
#include "GateFuncs.h"
#include "BlockCreation.h"
// fix these functions, they kind of suck and are fully vibe coded and probably aren't good. Fix them later and maybe add a parseString function that does the whole thing, also add support for 
void parseBlocks(const char *input, const char *owner) {
    const char *ptr = input;
    while (*ptr && *ptr != '?') {
        unsigned long int idTmp, sTmp;
        long int x, y, z;
        unsigned long int charsRead;

        // Read basic block info safely
        if (sscanf(ptr, "%d,%d,%f,%f,%f%n", &idTmp, &sTmp, &x, &y, &z, &charsRead) < 5)
            break;

        __uint8_t id = (__uint8_t)idTmp;
        bool s = (sTmp != 0);

        ptr += charsRead;

        if (BlockCount >= START_BLOCKS) {
            fprintf(stderr, "Error: too many blocks (max=%d)\n", START_BLOCKS);
            break;
        }

        // Create the block
        Block *b = CreateBlock(id, x, y, z);
        if (!b) break;

        blocks[BlockCount] = b;

        // Read extra data if present
        if (*ptr == ',') {
            ptr++; // skip comma
            const char *semi = strchr(ptr, ';');
            if (!semi) semi = strchr(ptr, '?');
            if (semi) {
                unsigned long int len = (unsigned long int)(semi - ptr);
                char *buffer = malloc(len + 1); // Dynamic buffer allocation
                Check_alloc_fail(buffer, break)
                memcpy(buffer, ptr, len);
                buffer[len] = '\0';

                switch (id) {
                    case FLIPFLOP: { // Flip Flop Block
                        FlipFlopBlock *flipFlopBlock = (FlipFlopBlock *)blocks[BlockCount];
                        flipFlopBlock->PrevXor = 1;
                        break;
                    }
                    case LED: { // LED Block
                        LedBlock *ledBlock = (LedBlock *)blocks[BlockCount];
                        ledBlock->RedAmount = 255;
                        ledBlock->GreenAmount = 255;
                        ledBlock->BlueAmount = 255;
                        ledBlock->OpacityOn = 100;
                        ledBlock->OpacityOff = 5;
                        ledBlock->analog = false;

                        unsigned long int values[6];
                        unsigned long int count = sscanf(buffer, "%d+%d+%d+%d+%d+%d",
                                           &values[0], &values[1], &values[2],
                                           &values[3], &values[4], &values[5]);
                        if (count >= 1) ledBlock->RedAmount = values[0];
                        if (count >= 2) ledBlock->GreenAmount = values[1];
                        if (count >= 3) ledBlock->BlueAmount = values[2];
                        if (count >= 4) ledBlock->OpacityOn = values[3];
                        if (count >= 5) ledBlock->OpacityOff = values[4];
                        if (count >= 6) ledBlock->analog = (values[5] != 0);
                        break;
                    }
                    case SOUND: { // Sound Block
                        SoundBlock *soundBlock = (SoundBlock *)blocks[BlockCount];
                        soundBlock->Frequency = 44000;
                        soundBlock->Instrument = 0;

                        unsigned long int values[2];
                        unsigned long int count = sscanf(buffer, "%d+%d", &values[0], &values[1]);
                        if (count >= 1) soundBlock->Frequency = values[0];
                        if (count >= 2) soundBlock->Instrument = values[1];
                        break;
                    }
                    case RANDOM: { // Random Block
                        RandomBlock *randomBlock = (RandomBlock *)blocks[BlockCount];
                        randomBlock->Probability = 50;
                        unsigned long int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            randomBlock->Probability = value;
                        break;
                    }
                    case TEXT: { // Text Block
                        CharBlock *charBlock = (CharBlock *)blocks[BlockCount];
                        charBlock->Character = 'A';
                        unsigned long int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            charBlock->Character = (char)value;
                        break;
                    }
                    case TILE: { // Tile Block
                        TileBlock *tileBlock = (TileBlock *)blocks[BlockCount];
                        tileBlock->RedAmount = 255;
                        tileBlock->GreenAmount = 255;
                        tileBlock->BlueAmount = 255;
                        tileBlock->Material = 0;
                        tileBlock->Collision = false;

                        unsigned long int values[5];
                        unsigned long int count = sscanf(buffer, "%d+%d+%d+%d+%d",
                                           &values[0], &values[1], &values[2],
                                           &values[3], &values[4]);
                        if (count >= 1) tileBlock->RedAmount = values[0];
                        if (count >= 2) tileBlock->GreenAmount = values[1];
                        if (count >= 3) tileBlock->BlueAmount = values[2];
                        if (count >= 4) tileBlock->Material = values[3];
                        if (count >= 5) tileBlock->Collision = (values[4] != 0);
                        break;
                    }
                    case DELAY: { // Delay Block
                        DelayBlock *delayBlock = (DelayBlock *)blocks[BlockCount];
                        delayBlock->DelayTime = 1;
                        unsigned long int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            delayBlock->DelayTime = value;
                        break;
                    }
                    case ANTENNA: { // Antenna Block
                        AntennaBlock *antennaBlock = (AntennaBlock *)blocks[BlockCount];
                        antennaBlock->Signal = 0;
                        antennaBlock->Global = false;

                        unsigned long int signal, global;
                        unsigned long int count = sscanf(buffer, "%d+%d", &signal, &global);
                        if (count >= 1) {
                            strncpy(antennaBlock->Owner, owner, sizeof(antennaBlock->Owner) - 1);
                            antennaBlock->Owner[sizeof(antennaBlock->Owner) - 1] = '\0';
                        }
                        if (count >= 2) antennaBlock->Signal = signal;
                        if (count >= 3) antennaBlock->Global = (global != 0);
                        break;
                    }
                    case LED_MIXER: { // LED Mixer Block
                        LedMixerBlock *ledMixerBlock = (LedMixerBlock *)blocks[BlockCount];
                        ledMixerBlock->Additive = true;
                        unsigned long int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            ledMixerBlock->Additive = (value != 0);
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

        // Update global arrays
        BlockCount++;
        state[BlockCount - 1] = s;
        preState[BlockCount - 1] = s;
    }
}

// Update parseConnections to use addInput/addOutput
void parseConnections(const char *input) {
    const char *ptr = input;
    while (*ptr && *ptr != '?') {
        unsigned long int from, to;
        unsigned long int charsRead = 0;
        
        if (sscanf(ptr, "%lu,%lu%n", &from, &to, &charsRead) < 2) break;
        ptr += charsRead;
        if (*ptr == ';') ptr++;

        from--;
        to--;
        if (from < 0 || from >= BlockCount || to < 0 || to >= BlockCount) {
            fprintf(stderr, "Warning: Invalid connection %d->%d (BlockCount=%d), skipping\n", from, to, BlockCount);
            continue;
        }

        Block *src = blocks[from];
        Block *dst = blocks[to];

        if (!src || !dst) {
            fprintf(stderr, "Warning: Null block in connection %d->%d, skipping\n", from, to);
            continue;
        }

        addOutput(src, to);
        addInput(dst, from);
    }
}