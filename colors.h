#ifndef _COLORS_H
#define _COLORS_H
#include <stdint.h>
#include <stdio.h>

#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define PURPLE 35
#define LIGHTBLUE 36
#define WHITE 37
#define STANDART 39

void coloredPrintf(uint8_t text_color, char s[]);
#endif
