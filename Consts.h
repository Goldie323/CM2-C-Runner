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
    LED_MIXER
}; //gate  values

enum {
    START_BLOCKS = 1024,
    MAX_THREADS = 20
};

typedef struct {
    unsigned long int *inputs;
    unsigned long int inputsSize;
    unsigned long int inputCount;
    unsigned long int *outputs;
    unsigned long int outputsSize;
    unsigned long int outputCount;
    unsigned long int x, y, z;
    __uint8_t OwnerID;
    __uint8_t ID;
} Block;

typedef struct {
    Block Base;
    bool PrevXor;
} FlipFlopBlock;

typedef struct {
    Block Base; //ID 6
    __uint8_t RedAmount;
    __uint8_t GreenAmount;
    __uint8_t BlueAmount;
    __uint8_t OpacityOn; // 5-100
    __uint8_t OpacityOff; // 5-100
    bool analog;
} LedBlock;

typedef struct {
    Block Base; //ID 7
    __uint32_t Frequency; // fixed point 0-16000 with 2 decimal places
    __uint8_t Instrument; // 0-5 for Sine, Square, Triangle Sawtooth, Meow, or Snare.
} SoundBlock;

typedef struct {
    Block Base; //ID 12
    __uint8_t Probability; //in percent 0-100
} RandomBlock;

typedef struct {
    Block Base; //ID 13
    char Character;
} CharBlock;

typedef struct {
    Block Base; //ID 14
    __uint8_t RedAmount;
    __uint8_t GreenAmount;
    __uint8_t BlueAmount;
    __uint8_t Material; // 0-13 for different material
    bool Collision;
} TileBlock;

typedef struct {
    Block Base; //ID 16
    __uint16_t DelayTime; // in ticks, 1-1000
} DelayBlock; 

typedef struct {
    Block Base; //ID 17
    __uint16_t Signal;
    bool Global;
} AntennaBlock;

typedef struct {
    Block Base; //ID 19
    bool Additive; // additive, subtractive blending
} LedMixerBlock; 

#endif