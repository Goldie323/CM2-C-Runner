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

struct flipFlopBlock{
    //ID 5
    uint8_t a;
    uint8_t b;
    bool PrevXor;
};

struct ledBlock{
    //ID 6
    uint_least8_t RedAmount;
    uint_least8_t GreenAmount;
    uint_least8_t BlueAmount;
    uint_least8_t OpacityOn; // 5-100
    uint_least8_t OpacityOff; // 5-100
    bool analog;
};

struct soundBlock{
    //ID 7
    uint_least32_t Frequency; // fixed point 0-16000 with 2 decimal places
    uint_least8_t Instrument; // 0-5 for Sine, Square, Triangle Sawtooth, Meow, or Snare.
};

struct randomBlock{
    //ID 12
    uint_least8_t Probability; //in percent 0-100
};

struct charBlock{
    //ID 13
    char Character;
};

struct tileBlock{
    //ID 14
    uint_least8_t RedAmount;
    uint_least8_t GreenAmount;
    uint_least8_t BlueAmount;
    uint_least8_t Material; // 0-13 for different material
    bool Collision;
};

struct delayBlock{
    //ID 16
    uint_least16_t DelayTime; // in ticks, 1-1000
}; 

struct antennaBlock{
    //ID 17
    uint_least16_t Signal;
    bool Global;
};

struct ledMixerBlock{
    //ID 19
    bool Additive; // additive, subtractive blending
}; 

union extraData{
    struct flipFlopBlock flipFlopBlock;
    struct ledBlock ledBlock;
    struct soundBlock soundBlock;
    struct randomBlock randomBlock;
    struct charBlock charBlock;
    struct tileBlock tileBlock;
    struct delayBlock delayBlock;
    struct antennaBlock antennaBlock;
    struct ledMixerBlock ledMixerBlock;
};

typedef struct {
    unsigned long int *inputs;
    unsigned long int inputsSize;
    unsigned long int inputCount;
    unsigned long int *outputs;
    unsigned long int outputsSize;
    unsigned long int outputCount;
    unsigned long int x, y, z;
    union extraData extraData;
    uint_least8_t OwnerID;
    uint_least8_t meta;
} block;



#endif