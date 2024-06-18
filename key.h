#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

#define KEY_NONE    0x00
#define KEY_SHIFT   0x11
#define KEY_SWITCH  0x12
#define KEY_ESCAPE  0x1B

uint8_t getKey();

#endif