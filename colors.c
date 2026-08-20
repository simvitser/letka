#include "colors.h"

void coloredPrintf(uint8_t text_color, char s[]) {
    printf("\033[%dm%s\033[0m", text_color, s);
}
