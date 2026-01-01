#ifndef METAMASKED_H
#define METAMASKED_H

#include <stdint.h>

enum metaBits {
    ID_MASK    = 0x1F, // 0001 1111
    BOOL0_MASK = 0x20, // 0010 0000
    BOOL1_MASK = 0x40  // 0100 0000
};

typedef uint_least8_t metaData;

static inline uint_least8_t getID(metaData meta) {
    return meta & ID_MASK;
}

static inline void setID(metaData *meta, uint_least8_t id) {
    *meta = (*meta & ~ID_MASK) | (id & ID_MASK);
    return;
}

static inline bool getBool0(metaData meta) {
    return (meta & BOOL0_MASK) != 0;
}

static inline void setBool0(metaData *meta, bool value) {
    if (value) *meta |= BOOL0_MASK;
    else *meta &= ~BOOL0_MASK;
    return;
}

static inline bool getBool1(metaData meta) {
    return (meta & BOOL1_MASK) != 0;
}

static inline void setBool1(metaData *meta, bool value) {
    if (value) *meta |= BOOL1_MASK;
    else *meta &= ~BOOL1_MASK;
    return;
}


static inline bool getState(metaData *meta, int flipBit) {
    if (!flipBit) return getBool0(*meta);
    else return getBool1(*meta);
}

static inline void setPrestate(metaData *meta, bool flipBit, bool value) {
    if (flipBit) setBool0(meta, value);
    else setBool1(meta, value);
    return;
}

static inline void setState(metaData *meta, bool flipBit, bool value) {
    if (!flipBit) setBool0(meta, value);
    else setBool1(meta, value);
    return;
}

static inline void setZero(metaData *meta) {
    *meta = 0;
    return;
}

static inline void setDead(metaData *meta) {
    *meta = 255;
    return;
}

static inline bool checkDead(metaData meta) {
    return (meta==255);
}

#endif