#ifndef Consts_h
#define Consts_h

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "Meta.h"

enum gateTypes{
    NOR,
    AND,
    OR,
    XOR,
    BUTTON,
    FLIPFLOP,
    LED,
    SOUND,
    CONDUCTOR,
    CUSTOM,
    NAND,
    XNOR,
    RANDOM,
    TEXT,
    TILE,
    NODE,
    DELAY,
    ANTENNA,
    CONDUCTOR_V2,
    LEDMIXER
}; //gate  values

enum {
    MAX_THREADS = 20 //should be set to twice the number of cores on your computer. This will probably be set up to a reasonable number later on but this is just to prevent dynamic memory for a simple array of the threads.
};

struct  FlipFlopBlock{
    //ID 5
    bool PrevXor;
};

struct LedBlock{
    //ID 6
    uint_least8_t RedAmount;
    uint_least8_t GreenAmount;
    uint_least8_t BlueAmount;
    uint_least8_t OpacityOn; // 5-100
    uint_least8_t OpacityOff; // 5-100
    bool analog;
};

struct  SoundBlock{
    //ID 7
    uint_least32_t Frequency; // fixed point 0-16000 with 2 decimal places
    uint_least8_t Instrument; // 0-5 for Sine, Square, Triangle Sawtooth, Meow, or Snare.
};

struct RandomBlock{
    //ID 12
    uint_least8_t Probability; //in percent 0-100
};

struct CharBlock{
    //ID 13
    char Character;
};

struct TileBlock{
    //ID 14
    uint_least8_t RedAmount;
    uint_least8_t GreenAmount;
    uint_least8_t BlueAmount;
    uint_least8_t Material; // 0-13 for different material
    bool Collision;
};

struct DelayBlock{
    //ID 16
    uint_least16_t DelayTime; // in ticks, 1-1000
}; 

struct AntennaBlock{
    //ID 17
    uint_least16_t Signal;
    bool Global;
};

struct LedMixerBlock{
    //ID 19
    bool Additive; // additive, subtractive blending
}; 

union extraData{
    struct FlipFlopBlock FlipFlopBlock;
    struct LedBlock LedBlock;
    struct SoundBlock SoundBlock;
    struct RandomBlock RandomBlock;
    struct CharBlock CharBlock;
    struct TileBlock TileBlock;
    struct DelayBlock DelayBlock;
    struct AntennaBlock AntennaBlock;
    struct LedMixerBlock LedMixerBlock;
};

typedef struct {
    uint_least64_t *Inputs;
    uint_least64_t InputsSize;
    uint_least64_t InputCount;
    uint_least64_t *Outputs;
    uint_least64_t OutputsSize;
    uint_least64_t OutputCount;
    uint_least64_t x, y, z;
    union extraData ExtraData;
    uint_least8_t OwnerID;
    uint_least8_t Meta;
} block;

typedef struct {
    block *BlockArray;
    uint_least64_t Capacity; //home much is allocated
    uint_least64_t Size; //how much is used
    uint_least64_t TickRate;
} Simulation;


#endif