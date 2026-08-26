#include <stdio.h>

#include "colors.h"

void coloredPrintf(uint8_t text_color, const char *s) {
    printf("\033[%dm%s\033[0m", text_color, s);
}
