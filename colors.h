#ifndef _COLORS_H
#define _COLORS_H
#include <stdint.h>

enum COLORS {
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    LIGHTBLUE,
    WHITE,
    STANDART = 39
};

void coloredPrintf(uint8_t text_color, const char *s);
#endif
