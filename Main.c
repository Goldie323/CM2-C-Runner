#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_BLOCKS 1024
#define MAX_CONNECTIONS 2048
#define MAX_THREADS 8

typedef struct {
    long int *inputs;
    long int inputsSize;
    long int inputCount;
    long int *outputs;
    long int outputsSize;
    long int outputCount;
    float x, y, z;
    __uint8_t ID;
} Block;

typedef struct {
    Block Base; //this is for block ID 6
    __uint8_t RedAmount; //0-255
    __uint8_t GreenAmount; // 0-255
    __uint8_t BlueAmount; // 0-255
    __uint8_t OpacityOn; // 5-100
    __uint8_t OpacityOff; // 5-100
    bool analog; // 0-1
} LedBlock; // these are all just for GUI and don't affect logic

typedef struct {
    Block Base; //this is for block ID 7
    __uint32_t Frequency; // fixed point 0-16000 with 2 decimal places
    __uint8_t Instrument; // 0-5 for Sine, Square, Triangle Sawtooth, Meow, or Snare.
} SoundBlock; // these are all just for GUI and don't affect logic

typedef struct {
    Block Base; //this is for block ID 12
    __uint8_t Probability; // 0.0 to 1.0, fixed point with 2 decimal places, so 0 to 100
} RandomBlock;

typedef struct {
    Block Base; //this is for block ID 13
    char Character; //stores a single character 0-255
} CharBlock; // these are all just for GUI and don't affect logic

typedef struct {
    Block Base; //this is for block ID 14
    __uint8_t RedAmount; //0-255
    __uint8_t GreenAmount; // 0-255
    __uint8_t BlueAmount; // 0-255
    __uint8_t Material; // 0-13 for different material
    bool Collision; // 0-1
} TileBlock; // these are all just for GUI and don't affect logic

typedef struct {
    Block Base; //this is for block ID 16
    __uint16_t DelayTime; // in ticks, 1-1000
} DelayBlock; 

typedef struct {
    Block Base; //this is for block ID 17
    char Owner[20]; // up to 20 character owner name
    __uint16_t Signal; // 0-65535, all antennas on the same signal will turn on if any are on
    bool Global; // 1 for global and all other global antennas will turn on if any are on
} AntennaBlock;

typedef struct {
    Block Base; //this is for block ID 19
    bool Additive; // 1 for additive blending, 0 for subtractive blending.
} LedMixerBlock; // these are all just for GUI and don't affect logic

Block *blocks[MAX_BLOCKS];
int BlockCount = 0;

bool stateArr[MAX_BLOCKS];
bool preStateArr[MAX_BLOCKS];
bool *state = stateArr;
bool *preState = preStateArr;


thrd_t threads[MAX_THREADS];
atomic_uint_least64_t taskIndex;
atomic_uint_least64_t blocksRemaining;


typedef bool (*GateFunc)(long int index);


bool norGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return false;
    }
    return true;
}

bool andGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (!state[inputs[i]]) return false;
    }
    return true;
}

bool orGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

bool xorGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    bool result = false;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) result = !result;
    }
    return result;
}

// gui will change the button state and then every tick will just turn it off and no inputs will change that
bool ButtonGate(long int index) {
    return false;
}

bool flipFlopButton(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return !state[index];
    }
    return state[index];
}

// logical or but gui handles led
bool ledGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// logical or but gui handles sound
bool soundGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// this needs to do a linear search of all the blocks to find the ones next to it and do an or operation to find it's current value
bool conductorGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

bool customGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

bool nandGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (!state[inputs[i]]) return true;
    }
    return false;
}

bool xnorGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    bool result = true;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) result = !result;
    }
    return result;
}

bool randomGate(long int index) {
    Block *b = blocks[index];
    RandomBlock *rb = (RandomBlock *)b;
    __uint8_t Probability = rb->Probability; // 0 to 100
    if (Probability == 0) return false;
    if (Probability >= 100) return true;

    int randomNumber = rand() / (RAND_MAX + 1.0) * 101;
    if (randomNumber < Probability) return true;
    return false;
}

bool TextGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// kind of like button, gui might tell it to be on but inputs don't change it and so every tick just turn it off
bool TileGate(long int index) {
    return false;
}

// this also requires a search, a better one but still need to search and it needs to check all it's outputs and check if they're a node and if they are, run the same logic on them. Needs recursion :sob:
bool nodeGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}


bool delayBlock(long int index) {
    //the original code was AI and dumb
    return false;
}


// need to do a linear search and check for all the globals/same user and then if ANY of them are on then it'll turn on, otherwise it's based on the OR of the inputs
bool antennaGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// another recursive one based on xyz and so I don't wanna do this yet because it's require a linear search of all the blocks and I don't wanna deal with that yes
bool conductor2Gate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

// does a logic or and the rest is handled by GUI
bool ledMixerGate(long int index) {
    const long int *inputs = blocks[index]->inputs;
    long int inputCount = blocks[index]->inputCount;
    for (long int i = 0; i < inputCount; i++) {
        if (state[inputs[i]]) return true;
    }
    return false;
}

GateFunc gateFunctions[256] = {NULL};

void initGateFunctions() {
    gateFunctions[0] = norGate;
    gateFunctions[1] = andGate;
    gateFunctions[2] = orGate;
    gateFunctions[3] = xorGate;
    gateFunctions[4] = ButtonGate;
    gateFunctions[5] = flipFlopButton;
    gateFunctions[6] = ledGate;
    gateFunctions[7] = soundGate;
    gateFunctions[8] = conductorGate;
    gateFunctions[9] = customGate;
    gateFunctions[10] = nandGate;
    gateFunctions[11] = xnorGate;
    gateFunctions[12] = randomGate;
    gateFunctions[13] = TextGate;
    gateFunctions[14] = TileGate;
    gateFunctions[15] = nodeGate;
    gateFunctions[16] = delayBlock;
    gateFunctions[17] = antennaGate;
    gateFunctions[18] = conductor2Gate;
    gateFunctions[19] = ledMixerGate;
}


void computeBlock(long int index);

atomic_bool terminateThreads = false;
int workerThread(void *arg) {
    (void)arg;
    while (!atomic_load(&terminateThreads)) {

        while (atomic_load(&blocksRemaining) == 0 && !atomic_load(&terminateThreads)) thrd_yield();

        int i = atomic_fetch_add(&taskIndex, 1);
        if (i >= BlockCount) break;
        computeBlock(i);
        atomic_fetch_sub(&blocksRemaining, 1);
    }
    return 0;
}
void computeBlock(long int index) {
    Block *b = blocks[index];
    if (!b) {
        return; // skip null block
    }

    GateFunc func = gateFunctions[b->ID];
    if (!func) {
        return;
    }

    bool result = func(index);
    
    preState[index] = result;
}

void tickCalc() {
    atomic_store(&taskIndex, 0);
    atomic_store(&blocksRemaining, BlockCount);

    // Main thread participates
    long int i;
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

void addInput(Block *b, long int value) {
    if (!b) return; // Safety check for null pointer
    if (b->inputsSize == 0) {
        b->inputsSize = 4;
        b->inputs = malloc(sizeof(long int) * b->inputsSize);
    } else if (b->inputCount >= b->inputsSize) {
        b->inputsSize *= 2;
        b->inputs = realloc(b->inputs, sizeof(long int) * b->inputsSize);
    }
    b->inputs[b->inputCount++] = value;
}

void addOutput(Block *b, long int value) {
    if (!b) return; // Safety check for null pointer
    if (b->outputsSize == 0) {
        b->outputsSize = 4;
        b->outputs = malloc(sizeof(long int) * b->outputsSize);
    } else if (b->outputCount >= b->outputsSize) {
        b->outputsSize *= 2;
        b->outputs = realloc(b->outputs, sizeof(long int) * b->outputsSize);
    }
    b->outputs[b->outputCount++] = value;
}

void removeInput(Block *b, long int value) {
    for (long int i = 0; i < b->inputCount; i++) {
        if (b->inputs[i] == value) {
            for (long int j = i; j < b->inputCount - 1; j++) {
                b->inputs[j] = b->inputs[j + 1];
            }
            b->inputCount--;
            break;
        }
    }
}

void removeOutput(Block *b, long int value) {
    for (long int i = 0; i < b->outputCount; i++) {
        if (b->outputs[i] == value) {
            for (long int j = i; j < b->outputCount - 1; j++) {
                b->outputs[j] = b->outputs[j + 1];
            }
            b->outputCount--;
            break;
        }
    }
}

void removeConnection(int from, int to) {
    Block *src = blocks[from];
    Block *dst = blocks[to];

    removeOutput(src, to);
    removeInput(dst, from);
}

void addConnection(int from, int to) {
    Block *src = blocks[from];
    Block *dst = blocks[to];

    addOutput(src, to);
    addInput(dst, from);
}

void removeBlock(long int index) {
    if (index < 0 || index >= BlockCount) return;

    Block *b = blocks[index];

    for (long int i = 0; i < b->inputCount; i++) {
        long int inputIdx = b->inputs[i];
        if (inputIdx >= 0 && inputIdx < BlockCount) {
            removeOutput(blocks[inputIdx], index);
        }
    }
    for (long int i = 0; i < b->outputCount; i++) {
        long int outputIdx = b->outputs[i];
        if (outputIdx >= 0 && outputIdx < BlockCount) {
            removeInput(blocks[outputIdx], index);
        }
    }

    free(b->inputs);
    free(b->outputs);
    free(b);

    BlockCount--;

    blocks[index] = NULL;

}

size_t extraDataSize(__uint8_t id) {
    switch (id) {
        case 6: return sizeof(LedBlock);
        case 7: return sizeof(SoundBlock); 
        case 12: return sizeof(RandomBlock);
        case 13: return sizeof(CharBlock);
        case 14: return sizeof(TileBlock) ;
        case 16: return sizeof(DelayBlock);
        case 17: return sizeof(AntennaBlock);
        case 19: return sizeof(LedMixerBlock);
        default: return sizeof(Block);
    }
}

Block *CreateBlock(__uint8_t id, float x, float y, float z, bool initialState) {
    if (BlockCount >= MAX_BLOCKS) return NULL;

    Block *b = malloc(extraDataSize(id));
    if (!b) return NULL;
    
    // Initialize the block
    memset(b, 0, extraDataSize(id));
    b->ID = id;
    b->x = x;
    b->y = y;
    b->z = z;
    b->inputs = NULL;
    b->outputs = NULL;
    b->inputsSize = 0;
    b->outputsSize = 0;
    b->inputCount = 0;
    b->outputCount = 0;

    return b;
}

void parseBlocks(const char *input, const char *owner) {
    const char *ptr = input;
    while (*ptr && *ptr != '?') {
        int idTmp, sTmp;
        float x, y, z;
        int charsRead;

        // Read basic block info safely
        if (sscanf(ptr, "%d,%d,%f,%f,%f%n", &idTmp, &sTmp, &x, &y, &z, &charsRead) < 5)
            break;

        __uint8_t id = (__uint8_t)idTmp;
        bool s = (sTmp != 0);

        ptr += charsRead;

        if (BlockCount >= MAX_BLOCKS) {
            fprintf(stderr, "Error: too many blocks (max=%d)\n", MAX_BLOCKS);
            break;
        }

        // Create the block
        Block *b = CreateBlock(id, x, y, z, s);
        if (!b) break;

        blocks[BlockCount] = b;

        // Read extra data if present
        if (*ptr == ',') {
            ptr++; // skip comma
            const char *semi = strchr(ptr, ';');
            if (!semi) semi = strchr(ptr, '?');
            if (semi) {
                int len = (int)(semi - ptr);
                char *buffer = malloc(len + 1); // Dynamic buffer allocation
                if (!buffer) {
                    fprintf(stderr, "Error: Failed to allocate memory for buffer\n");
                    break;
                }
                memcpy(buffer, ptr, len);
                buffer[len] = '\0';

                switch (id) {
                    case 6: { // LED Block
                        LedBlock *ledBlock = (LedBlock *)blocks[BlockCount];
                        ledBlock->RedAmount = 255;
                        ledBlock->GreenAmount = 255;
                        ledBlock->BlueAmount = 255;
                        ledBlock->OpacityOn = 100;
                        ledBlock->OpacityOff = 5;
                        ledBlock->analog = false;

                        int values[6];
                        int count = sscanf(buffer, "%d+%d+%d+%d+%d+%d",
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
                    case 7: { // Sound Block
                        SoundBlock *soundBlock = (SoundBlock *)blocks[BlockCount];
                        soundBlock->Frequency = 44000;
                        soundBlock->Instrument = 0;

                        int values[2];
                        int count = sscanf(buffer, "%d+%d", &values[0], &values[1]);
                        if (count >= 1) soundBlock->Frequency = values[0];
                        if (count >= 2) soundBlock->Instrument = values[1];
                        break;
                    }
                    case 12: { // Random Block
                        RandomBlock *randomBlock = (RandomBlock *)blocks[BlockCount];
                        randomBlock->Probability = 50;
                        int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            randomBlock->Probability = value;
                        break;
                    }
                    case 13: { // Char Block
                        CharBlock *charBlock = (CharBlock *)blocks[BlockCount];
                        charBlock->Character = 'A';
                        int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            charBlock->Character = (char)value;
                        break;
                    }
                    case 14: { // Tile Block
                        TileBlock *tileBlock = (TileBlock *)blocks[BlockCount];
                        tileBlock->RedAmount = 255;
                        tileBlock->GreenAmount = 255;
                        tileBlock->BlueAmount = 255;
                        tileBlock->Material = 0;
                        tileBlock->Collision = false;

                        int values[5];
                        int count = sscanf(buffer, "%d+%d+%d+%d+%d",
                                           &values[0], &values[1], &values[2],
                                           &values[3], &values[4]);
                        if (count >= 1) tileBlock->RedAmount = values[0];
                        if (count >= 2) tileBlock->GreenAmount = values[1];
                        if (count >= 3) tileBlock->BlueAmount = values[2];
                        if (count >= 4) tileBlock->Material = values[3];
                        if (count >= 5) tileBlock->Collision = (values[4] != 0);
                        break;
                    }
                    case 16: { // Delay Block
                        DelayBlock *delayBlock = (DelayBlock *)blocks[BlockCount];
                        delayBlock->DelayTime = 1;
                        int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            delayBlock->DelayTime = value;
                        break;
                    }
                    case 17: { // Antenna Block
                        AntennaBlock *antennaBlock = (AntennaBlock *)blocks[BlockCount];
                        antennaBlock->Signal = 0;
                        antennaBlock->Global = false;

                        int signal, global;
                        int count = sscanf(buffer, "%d+%d", &signal, &global);
                        if (count >= 1) {
                            strncpy(antennaBlock->Owner, owner, sizeof(antennaBlock->Owner) - 1);
                            antennaBlock->Owner[sizeof(antennaBlock->Owner) - 1] = '\0';
                        }
                        if (count >= 2) antennaBlock->Signal = signal;
                        if (count >= 3) antennaBlock->Global = (global != 0);
                        break;
                    }
                    case 19: { // LED Mixer Block
                        LedMixerBlock *ledMixerBlock = (LedMixerBlock *)blocks[BlockCount];
                        ledMixerBlock->Additive = true;
                        int value;
                        if (sscanf(buffer, "%d", &value) == 1)
                            ledMixerBlock->Additive = (value != 0);
                        break;
                    }
                    default:
                        break;
                }
                free(buffer); // Free the dynamically allocated buffer
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
        int from, to;
        int charsRead;
        if (sscanf(ptr, "%d,%d%n", &from, &to, &charsRead) < 2) break;
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

void sleepForTick(double seconds) {
    struct timespec req, rem;
    req.tv_sec = (time_t)seconds;
    req.tv_nsec = (long)((seconds - req.tv_sec) * 1e9);
    while (nanosleep(&req, &rem) == -1) {
        req = rem; // Continue sleeping for the remaining time
    }
}


int main(int argc, char *argv[]) {

    for (long int i = 0; i < MAX_THREADS; i++) {
        thrd_create(&threads[i], workerThread, NULL);
    }

    // Get circuit input string from command line arguments
    //const char *input = argv[1];

    const char *input = "0,0,-4,0,18,;0,0,-3,0,18,;0,0,-2,0,18,;0,0,-1,0,18,;0,0,0,0,18,;0,0,1,0,18,;0,0,2,0,18,;0,0,3,0,18,;0,0,4,0,18,;0,0,5,0,18,;2,0,-4,0,16,;2,0,-3,0,16,;2,0,-2,0,16,;2,0,-1,0,16,;2,0,0,0,16,;2,0,1,0,16,;2,0,2,0,16,;2,0,3,0,16,;2,0,4,0,16,;2,0,5,0,16,;2,0,-4,0,14,;2,0,-3,0,14,;2,0,-2,0,14,;2,0,-1,0,14,;2,0,0,0,14,;2,0,1,0,14,;2,0,2,0,14,;2,0,3,0,14,;2,0,4,0,14,;2,0,5,0,14,;2,0,-4,0,12,;2,0,-3,0,12,;2,0,-2,0,12,;2,0,-1,0,12,;2,0,0,0,12,;2,0,1,0,12,;2,0,2,0,12,;2,0,3,0,12,;2,0,4,0,12,;2,0,5,0,12,;2,0,-4,0,10,;2,0,-3,0,10,;2,0,-2,0,10,;2,0,-1,0,10,;2,0,0,0,10,;2,0,1,0,10,;2,0,2,0,10,;2,0,3,0,10,;2,0,4,0,10,;2,0,5,0,10,;2,0,-4,0,8,;2,0,-3,0,8,;2,0,-2,0,8,;2,0,-1,0,8,;2,0,0,0,8,;2,0,1,0,8,;2,0,2,0,8,;2,0,3,0,8,;2,0,4,0,8,;2,0,5,0,8,;2,0,-4,0,6,;2,0,-3,0,6,;2,0,-2,0,6,;2,0,-1,0,6,;2,0,0,0,6,;2,0,1,0,6,;2,0,2,0,6,;2,0,3,0,6,;2,0,4,0,6,;2,0,5,0,6,;2,0,-4,0,4,;2,0,-3,0,4,;2,0,-2,0,4,;2,0,-1,0,4,;2,0,0,0,4,;2,0,1,0,4,;2,0,2,0,4,;2,0,3,0,4,;2,0,4,0,4,;2,0,5,0,4,;2,0,-4,0,2,;2,0,-3,0,2,;2,0,-2,0,2,;2,0,-1,0,2,;2,0,0,0,2,;2,0,1,0,2,;2,0,2,0,2,;2,0,3,0,2,;2,0,4,0,2,;2,0,5,0,2,;2,0,-4,0,0,;2,0,-3,0,0,;2,0,-2,0,0,;2,0,-1,0,0,;2,0,0,0,0,;2,0,1,0,0,;2,0,2,0,0,;2,0,3,0,0,;2,0,4,0,0,;2,0,5,0,0,;2,0,-4,0,-2,;2,0,-3,0,-2,;2,0,-2,0,-2,;2,0,-1,0,-2,;2,0,0,0,-2,;2,0,1,0,-2,;2,0,2,0,-2,;2,0,3,0,-2,;2,0,4,0,-2,;2,0,5,0,-2,;2,0,-4,0,-4,;2,0,-3,0,-4,;2,0,-2,0,-4,;2,0,-1,0,-4,;2,0,0,0,-4,;2,0,1,0,-4,;2,0,2,0,-4,;2,0,3,0,-4,;2,0,4,0,-4,;2,0,5,0,-4,;2,0,-4,0,-6,;2,0,-3,0,-6,;2,0,-2,0,-6,;2,0,-1,0,-6,;2,0,0,0,-6,;2,0,1,0,-6,;2,0,2,0,-6,;2,0,3,0,-6,;2,0,4,0,-6,;2,0,5,0,-6,;2,0,-4,0,-8,;2,0,-3,0,-8,;2,0,-2,0,-8,;2,0,-1,0,-8,;2,0,0,0,-8,;2,0,1,0,-8,;2,0,2,0,-8,;2,0,3,0,-8,;2,0,4,0,-8,;2,0,5,0,-8,;2,0,-4,0,-10,;2,0,-3,0,-10,;2,0,-2,0,-10,;2,0,-1,0,-10,;2,0,0,0,-10,;2,0,1,0,-10,;2,0,2,0,-10,;2,0,3,0,-10,;2,0,4,0,-10,;2,0,5,0,-10,;2,0,-4,0,-12,;2,0,-3,0,-12,;2,0,-2,0,-12,;2,0,-1,0,-12,;2,0,0,0,-12,;2,0,1,0,-12,;2,0,2,0,-12,;2,0,3,0,-12,;2,0,4,0,-12,;2,0,5,0,-12,;2,0,-4,0,-14,;2,0,-3,0,-14,;2,0,-2,0,-14,;2,0,-1,0,-14,;2,0,0,0,-14,;2,0,1,0,-14,;2,0,2,0,-14,;2,0,3,0,-14,;2,0,4,0,-14,;2,0,5,0,-14,;2,0,-4,0,-16,;2,0,-3,0,-16,;2,0,-2,0,-16,;2,0,-1,0,-16,;2,0,0,0,-16,;2,0,1,0,-16,;2,0,2,0,-16,;2,0,3,0,-16,;2,0,4,0,-16,;2,0,5,0,-16,;2,0,-4,0,-18,;2,0,-3,0,-18,;2,0,-2,0,-18,;2,0,-1,0,-18,;2,0,0,0,-18,;2,0,1,0,-18,;2,0,2,0,-18,;2,0,3,0,-18,;2,0,4,0,-18,;2,0,5,0,-18,;2,0,-4,0,-20,;2,0,-3,0,-20,;2,0,-2,0,-20,;2,0,-1,0,-20,;2,0,0,0,-20,;2,0,1,0,-20,;2,0,2,0,-20,;2,0,3,0,-20,;2,0,4,0,-20,;2,0,5,0,-20,?11,21;12,22;13,23;14,24;15,25;16,26;17,27;18,28;19,29;20,30;21,31;22,32;23,33;24,34;25,35;26,36;27,37;28,38;29,39;30,40;41,51;42,52;43,53;44,54;45,55;46,56;47,57;48,58;49,59;50,60;31,41;32,42;33,43;34,44;35,45;36,46;37,47;38,48;39,49;40,50;51,61;52,62;53,63;54,64;55,65;56,66;57,67;58,68;59,69;60,70;61,71;62,72;63,73;64,74;65,75;66,76;67,77;68,78;69,79;70,80;71,81;72,82;73,83;74,84;75,85;76,86;77,87;78,88;79,89;80,90;81,91;82,92;83,93;84,94;85,95;86,96;87,97;88,98;89,99;90,100;91,101;92,102;93,103;94,104;95,105;96,106;97,107;98,108;99,109;100,110;101,111;102,112;103,113;104,114;105,115;106,116;107,117;108,118;109,119;110,120;111,121;112,122;113,123;114,124;115,125;116,126;117,127;118,128;119,129;120,130;121,131;122,132;123,133;124,134;125,135;126,136;127,137;128,138;129,139;130,140;131,141;132,142;133,143;134,144;135,145;136,146;137,147;138,148;139,149;140,150;141,151;142,152;143,153;144,154;145,155;146,156;147,157;148,158;149,159;150,160;151,161;152,162;153,163;154,164;155,165;156,166;157,167;158,168;159,169;160,170;161,171;162,172;163,173;164,174;165,175;166,176;167,177;168,178;169,179;170,180;171,181;172,182;173,183;174,184;175,185;176,186;177,187;178,188;179,189;180,190;181,191;182,192;183,193;184,194;185,195;186,196;187,197;188,198;189,199;190,200;191,1;192,2;193,3;194,4;195,5;196,6;197,7;198,8;199,9;200,10;1,11;2,12;3,13;4,14;5,15;6,16;7,17;8,18;9,19;10,20??";

    const char *q1 = strchr(input, '?');
    if (q1) {
        size_t blockDataLen = q1 - input;
        char *blockData = malloc(blockDataLen + 1);
        if (!blockData) {
            fprintf(stderr, "Error: Failed to allocate memory for block data\n");
            return 1;
        }
        strncpy(blockData, input, blockDataLen);
        blockData[blockDataLen] = '\0';
        parseBlocks(blockData, "Goldie323");
        free(blockData);

        parseConnections(q1+1);
    }

    initGateFunctions();
    
    // Get simulation parameters from command line arguments
    //long int totalTicks = atol(argv[2]);
    //long int ticksPerSecond = atol(argv[3]);
    

    long int totalTicks = 1000;
    long int ticksPerSecond = 0;

    if (totalTicks <= 0) {
        fprintf(stderr, "Error: total ticks must be positive\n");
        return 1;
    }
    
    double tickDuration = (ticksPerSecond > 0) ? 1.0 / ticksPerSecond : 0;
    
    for (long int t = 0; t < totalTicks; t++) {
        clock_t start = (ticksPerSecond > 0) ? clock() : 0;
        tick();
        
        if (ticksPerSecond > 0) {
            clock_t end = clock();
            double elapsed = (double)(end - start)/CLOCKS_PER_SEC;
            if (elapsed < tickDuration) sleepForTick(tickDuration - elapsed);
        }

        // Print tick and block states on the same line, updating dynamically
        printf("\rTick %lld | ", t+1);
        for (long int i = 0; i < BlockCount; i++) {
            printf("%dB%lld:%d ", blocks[i]->ID, i, state[i]);
        }
        fflush(stdout);
    }
    printf("\n"); // Add newline after all ticks are done

    // Signal threads to terminate
    atomic_store(&terminateThreads, true);
    atomic_store(&blocksRemaining, 0); // Reset to 0 to allow threads to exit

    // Join threads
    for (long int i = 0; i < MAX_THREADS; i++) {
        printf("\rJoining threads %ld/%ld", i+1, MAX_THREADS);
        fflush(stdout);
        thrd_join(threads[i], NULL);
    }
    printf("\rAll threads joined                    \n");

    // Free dynamic memory
    for (int i = 0; i < BlockCount; i++) {
        printf("\rFreeing blocks %d/%d", i+1, BlockCount);
        fflush(stdout);
        free(blocks[i]->inputs);
        free(blocks[i]->outputs);
        free(blocks[i]);
    }
    printf("\rAll blocks freed                    \n");
    return 0;
}
