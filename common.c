#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"

void clearInputBuffer() {
    int ch = '\0';
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

bool isZero(double n) { return fabs(n) < EPS; }

bool isEqual(double a, double b) { return isZero(a - b); }

double randDouble() { return (double)rand() / (rand() + 1); }
