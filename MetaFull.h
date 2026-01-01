#ifndef METAFULL_H
#define METAFULL_H

#include <stdint.h>
#include <stdbool.h>


typedef struct {
    bool bools[2];
    uint_least8_t id;
} metaData;

static inline uint_least8_t getID(metaData meta) {
    return meta.id;
}

static inline void setID(metaData *meta, uint_least8_t id) {
    meta->id = id;
    return;
}

static inline bool getBool0(metaData meta) {
    return meta.bools[0];
}

static inline void setBool0(metaData *meta, bool value) {
    meta->bools[0] = value;
    return;
}

static inline bool getBool1(metaData meta) {
    return meta.bools[1];
}

static inline void setBool1(metaData *meta, bool value) {
    meta->bools[1] = value;
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
    meta->bools[0] = 0;
    meta->bools[1] = 0;
    meta->id = 0;
}

static inline void setDead(metaData *meta) {
    meta->bools[0] = 0;
    meta->bools[1] = 0;
    meta->id = 255;
}

static inline bool checkDead(metaData meta) {
    return (meta.id==255);
}



#endif