#ifndef _COMMON_H
#define _COMMON_H
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#define EPS 1e-9
#define STATIC_STRLEN(s) (sizeof(s) - 1)

void clearInputBuffer();
bool isZero(double n);
bool isEqual(double a, double b);
double randDouble();

#endif
