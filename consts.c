enum {
    NOR = 0,
    AND = 1,
    OR = 2,
    XOR = 3,
    BUTTON = 4,
    FLIPFLOP = 5,
    LED = 6,
    SOUND = 7,
    CONDUCTOR = 8,
    CUSTOM = 9,
    NAND = 10,
    XNOR = 11,
    RANDOM = 12,
    TEXT = 13,
    TILE = 14,
    NODE = 15,
    DELAY = 16,
    ANTENNA = 17,
    CONDUCTOR_V2 = 18,
    LED_MIXER = 19
}; //gate  values

enum {
    START_BLOCKS = 1024,
    MAX_THREADS = 3 //doesn't include main so for the real amount of threads add 1 and you get the real amount including main.
};

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