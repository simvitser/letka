#ifndef _COMMON_H
#define _COMMON_H
#include <stdbool.h>

#define EPS 1e-9
#define STATIC_STRLEN(s) (sizeof(s) - 1)
#define STR2(x) #x
#define STR(x) STR2(x) //TODO: спросить Деда

void clearInputBuffer();
bool isZero(double n);
bool isEqual(double a, double b);
double randDouble();

#endif
