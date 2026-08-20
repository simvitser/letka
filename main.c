#define TESTS

#include "colors.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

const double EPS = 0.001;

typedef enum { ZERO_SOLVES, ONE_SOLVES, TWO_SOLVES, INF_SOLVES } KSolves;

void clearInputBuffer() {
    char ch = 0;
    while ((ch = getchar()) != '\n')
        ;
}

bool getKoefs(double *a, double *b, double *c);
KSolves solveLinear(double k, double b, double *x);
KSolves solveSquare(double a, double b, double c, double *x1, double *x2);
void printSolves(KSolves n, double x1, double x2);
bool runUnittest(double a, double b, double c, KSolves n, double x1, double x2);

int main() {
    coloredPrintf(RED, "Alya is the best\n");
    coloredPrintf(GREEN, "Alya is the best\n");
#ifdef TESTS
    assert(runUnittest(0, 0, 0, INF_SOLVES, 0, 0) && "comment");
    assert(runUnittest(4, 5, 6, ZERO_SOLVES, 0, 0));
    assert(runUnittest(1, 2, 1, ONE_SOLVES, -1, 0));
    assert(runUnittest(1, 3, 1, TWO_SOLVES, -2.61803, -0.381966));
    assert(runUnittest(34, 444, 34, TWO_SOLVES, -12.9818, -0.077031));
    assert(runUnittest(7, 878, 7, TWO_SOLVES, -125.421, -0.00797317));
#endif
    puts("This not AI-generated reshalka");
    double a = NAN, b = NAN, c = NAN;
    while (!getKoefs(&a, &b, &c)) {
        double x1 = NAN, x2 = NAN;
        KSolves n = solveSquare(a, b, c, &x1, &x2);
        printSolves(n, x1, x2);
    }
    puts("Bye bye! ;)");
    return 0;
}

bool getKoefs(double *a, double *b, double *c) {
    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);

    printf("enter a b c\n>>> ");
    char ch = 0;
    while (scanf("%lg %lg %lg", a, b, c) != 3) {
        printf("NO. enter a b c\n>>> ");
        while ((ch = getchar()) != '\n') {
            if (ch == 'q' || ch == EOF)
                return 1;
        }
    }
    clearInputBuffer();
    return 0;
}

KSolves solveLinear(double k, double b, double *x) {
    if (fabs(k) <= EPS) {
        if (fabs(b) <= EPS)
            return INF_SOLVES;
        return ZERO_SOLVES;
    }
    *x = -b / k;
    return ONE_SOLVES;
}

KSolves solveSquare(double a, double b, double c, double *x1, double *x2) {
    assert(isfinite(a));
    assert(isfinite(b));
    assert(isfinite(c));
    assert(x1 != NULL);
    assert(x2 != NULL);

    if (fabs(a) <= EPS) {
        return solveLinear(b, c, x1);
    }

    double D = b * b - 4 * a * c;
    if (D < -EPS) {
        return ZERO_SOLVES;
    } else if (D < EPS) {
        *x1 = -b / 2 / a;
        *x2 = *x1;
        return ONE_SOLVES;
    } else {
        double sqrtD = sqrt(D);
        *x1 = (-b - sqrtD) / 2 / a;
        *x2 = (-b + sqrtD) / 2 / a;
        return TWO_SOLVES;
    }
}

void printSolves(KSolves n, double x1, double x2) {
    switch (n) {
    case ZERO_SOLVES:
        puts("NO solves");
        break;
    case ONE_SOLVES:
        printf("%lg\n", x1);
        break;
    case TWO_SOLVES:
        printf("%lg %lg\n", x1, x2);
        break;
    case INF_SOLVES:
        puts("ALL IN");
        break;
    }
}

bool runUnittest(double a, double b, double c, KSolves n, double x1,
                 double x2) {
    double x1_test = NAN, x2_test = NAN;
    KSolves n_test = solveSquare(a, b, c, &x1_test, &x2_test);
    if (n != n_test) {
        return false;
    }
    switch (n) {
    case ZERO_SOLVES:
        return true;
    case ONE_SOLVES:
        return fabs(x1 - x1_test) < EPS;
    case TWO_SOLVES:
        return fabs(x1 - x1_test) < EPS && fabs(x2 - x2_test) < EPS;
    case INF_SOLVES:
        return true;
    }
}
