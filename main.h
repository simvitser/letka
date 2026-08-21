#ifndef _MAIN_H
#define _MAIN_H

#include "colors.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const double EPS = 0.001;

typedef enum { ZERO_SOLVES, ONE_SOLVES, TWO_SOLVES, INF_SOLVES } KSolves;

/*!
 * функция для очистки входного буфера
 * считывает все символы до \n
 */
void clearInputBuffer();
bool isZero(double n);
/*!
 * функция для ввода 3 чисел с плавающей точкой
 * @param[in] a - старший коэффициент
 * @param[in] b - коэффициент
 * @param[in] c - свободный член
 */
void parseArgs(int argc, char const *const *argv);
void processFlagChar(const char fl);
void processFlagString(const char *fl);
bool getKoefs(double *a, double *b, double *c);
KSolves solveLinear(double k, double b, double *x);
KSolves solveSquare(double a, double b, double c, double *x1, double *x2);
void printSolves(KSolves solution_type, double x1, double x2);
bool runUnittest(double a, double b, double c, KSolves solution_type, double x1,
                 double x2);

#endif
