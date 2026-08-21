#ifndef _MAIN_H
#define _MAIN_H

#include "colors.h"
#include <assert.h>
#include <curses.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define myassert(esp)                                                          \
    if (!(esp)) {                                                              \
        fprintf(stderr, "ASSSSSERT: %s! line: %d, file: %s, func: %s\n", #esp, \
                __LINE__, __FILE__, __func__);                                 \
        abort();                                                               \
    }

const double EPS = 0.001;
const uint8_t MAX_ARGS_PER_FLAG =
    10; // максимальное количество аргументов у флага из командной строки

typedef enum { ZERO_SOLVES, ONE_SOLVE, TWO_SOLVES, INF_SOLVES } KSolves;

/*!
 * функция для очистки входного буфера
 * считывает все символы до \n
 */
void runUnittests();
void runUnittestsFromFile(const char *filename);
void clearInputBuffer();
bool isZero(double n);
bool isEqual(double a, double b);
/*!
 * функция для ввода 3 чисел с плавающей точкой
 * @param[in] a - старший коэффициент
 * @param[in] b - коэффициент
 * @param[in] c - свободный член
 */
void parseArgs(int argc, char const *const *argv);
void processFlagString(const int argc, char const *const *args);
bool getKoefs(double *a, double *b, double *c);
KSolves solveLinear(double k, double b, double *x);
KSolves solveSquare(double a, double b, double c, double *x1, double *x2);
void printSolves(KSolves solution_type, double x1, double x2);
bool runUnittest(double a, double b, double c, KSolves solution_type, double x1,
                 double x2);

#endif
