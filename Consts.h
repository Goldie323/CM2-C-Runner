#ifndef Consts_h
#define Consts_h

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
enum {
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
    MAX_THREADS = 20
};

#define ID_MASK     0x1F  // 0001 1111
#define BOOL0_MASK  0x20  // 0010 0000
#define BOOL1_MASK  0x40  // 0100 0000

typedef struct block{
    struct block **inputs;
    unsigned long int inputsSize;
    unsigned long int inputCount;
    struct block **outputs;
    unsigned long int outputsSize;
    unsigned long int outputCount;
    unsigned long int x, y, z;
    struct block *next;
    uint_least8_t OwnerID;
    uint_least8_t meta;
} block;

typedef struct {
    block Base;
    bool PrevXor;
} flipFlopBlock;

typedef struct {
    block Base; //ID 6
    uint_least8_t RedAmount;
    uint_least8_t GreenAmount;
    uint_least8_t BlueAmount;
    uint_least8_t OpacityOn; // 5-100
    uint_least8_t OpacityOff; // 5-100
    bool analog;
} ledBlock;

typedef struct {
    block Base; //ID 7
    uint_least32_t Frequency; // fixed point 0-16000 with 2 decimal places
    uint_least8_t Instrument; // 0-5 for Sine, Square, Triangle Sawtooth, Meow, or Snare.
} soundBlock;

typedef struct {
    block Base; //ID 12
    uint_least8_t Probability; //in percent 0-100
} randomBlock;

typedef struct {
    block Base; //ID 13
    char Character;
} charBlock;

typedef struct {
    block Base; //ID 14
    uint_least8_t RedAmount;
    uint_least8_t GreenAmount;
    uint_least8_t BlueAmount;
    uint_least8_t Material; // 0-13 for different material
    bool Collision;
} tileBlock;

typedef struct {
    block Base; //ID 16
    uint_least16_t DelayTime; // in ticks, 1-1000
} delayBlock; 

typedef struct {
    block Base; //ID 17
    uint_least16_t Signal;
    bool Global;
} antennaBlock;

typedef struct {
    block Base; //ID 19
    bool Additive; // additive, subtractive blending
} ledMixerBlock; 

#endif