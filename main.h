#ifndef _MAIN_H
#define _MAIN_H

#include "colors.h"
#include "hash.h"
#include <assert.h>
#include <curses.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// #define quietColoredPrintf(color, smth) if (!GLOBAL_QUIET) coloredPrintf(color, smth)
// #define quietPrintf(smth) if (!GLOBAL_QUIET) printf(smth)    // old version
#define QUIET if (!GLOBAL_QUIET) 
#define myassert(esp)                                                          \
    if (!(esp)) {                                                              \
        fprintf(stderr, "ASSSSSERT: %s! line: %d, file: %s, func: %s\n", #esp, \
                __LINE__, __FILE__, __func__);                                 \
        abort();                                                               \
    }
 
#define MAX_ARGS_PER_FLAG 10 // максимальное количество аргументов у флага из командной строки
#define MAX_LEN 100
#define SIZE_PLOT_Y 41
#define SIZE_PLOT_X 121

typedef enum {
    ZERO_SOLVES,
    ONE_SOLVE,
    TWO_SOLVES,
    INF_SOLVES
} KSolves;

typedef struct {
    double a, b, c;
} SquareKoefs;


enum FLAG_BYTES {
    FLAG_QUIET = (1U << 0), 
    FLAG_TYPEENTER = (1U << 1), 
    FLAG_TESTFAILED = (1U << 2), 
    FLAG_DEBUGARGS = (1U << 3), 
    FLAG_DEBUGPARSE = (1U << 4), 
    FLAG_SIGNUP = (1U << 5), 
    FLAG_RANDOMTESTFAILED = (1U << 6),
    FLAG_DRAWPLOT = (1U << 7),
    FLAG_DRAWPLOTOFFSET = (1U << 8)
};


// enum FLAG_BYTES {
//     FLAG_QUIET = 0, 
//     FLAG_TYPEENTER, 
//     FLAG_TESTFAILED, 
//     FLAG_DEBUGARGS, 
//     FLAG_DEBUGPARSE, 
//     FLAG_SIGNUP, 
//     FLAG_RANDOMTESTFAILED,
//     FLAG_DRAWPLOT,
//     FLAG_DRAWPLOTOFFSET
// };   // old

enum MAINERRORS {
    MAINERRORS_FAILEDTESTS = 67,
    MAINERRORS_FAILEDRANDOMTEST
};

/*!
 * функция для очистки входного буфера
 * считывает все символы до \n
 */
void runUnittests();
bool runUnittestsFromFile(const char *filename);

/*!
 * функция для ввода 3 чисел с плавающей точкой
 * @param[in] a - старший коэффициент
 * @param[in] b - коэффициент
 * @param[in] c - свободный член
 */
uint64_t parseArgs(int argc, char const *const *argv);
void processFlagString(const int argc, char const *const *args, uint64_t *flags);
bool getKoefsOld(double *a, double *b, double *c);
bool getKoefsNew(double *a, double *b, double *c);
bool parseKoefs(char *s, double *a, double *b, double *c);
bool getKoefs(double *a, double *b, double *c, bool typeenter);
KSolves solveLinear(double k, double b, double *x);
KSolves solveSquare(double a, double b, double c, double *x1, double *x2);
void printSolves(KSolves solution_type, double x1, double x2);
bool runUnittest(double a, double b, double c, KSolves solution_type, double x1, double x2);
double countEq(double a, double b, double c, double x);
bool runRandomUnittest(long num_tests);
void drawPlot(double a, double b, double c);
void drawPlotOffset(double a, double b, double c);

#endif
