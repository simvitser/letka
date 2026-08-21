#include <string.h>
#define TESTS

#include "main.h"
bool QUIET = false;

int main(const int argc, const char *const *argv) {
    parseArgs(argc, argv);
    if (!QUIET) {
        coloredPrintf(RED, "Alya is the best\n");
        coloredPrintf(GREEN, "Alya is the best\n");
        puts("This not AI-generated reshalka");
    }
#ifdef TESTS
    assert(runUnittest(0, 0, 0, INF_SOLVES, 0, 0));
    assert(runUnittest(4, 5, 6, ZERO_SOLVES, 0, 0));
    assert(runUnittest(1, 2, 1, ONE_SOLVES, -1, 0));
    assert(runUnittest(1, 3, 1, TWO_SOLVES, -2.61803, -0.381966));
    assert(runUnittest(34, 444, 34, TWO_SOLVES, -12.9818, -0.077031));
    assert(runUnittest(7, 878, 7, TWO_SOLVES, -125.421, -0.00797317));
#endif
    double a = NAN, b = NAN, c = NAN;
    while (!getKoefs(&a, &b, &c)) {
        double x1 = NAN, x2 = NAN;
        KSolves solution_type = solveSquare(a, b, c, &x1, &x2);
        printSolves(solution_type, x1, x2);
    }
    if (!QUIET)
        puts("Bye bye! ;)");
    return 0;
}

void clearInputBuffer() {
    char ch = 0;
    while ((ch = getchar()) != '\n')
        ;
}

bool getKoefs(double *a, double *b, double *c) {
    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    if (!QUIET)
        printf("enter a b c\n>>> ");
    char ch = 0;
    while (scanf("%lg %lg %lg", a, b, c) != 3) {
        if (!QUIET)
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
    assert(x1 != x2);

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

void printSolves(KSolves solution_type, double x1, double x2) {
    switch (solution_type) {
    case ZERO_SOLVES:
        puts("NO solves");
        break;
    case ONE_SOLVES:
        printf("%lg -\n", x1);
        break;
    case TWO_SOLVES:
        printf("%lg %lg\n", x1, x2);
        break;
    case INF_SOLVES:
        puts("ALL IN");
        break;
    default:
        assert(false);
    }
}

bool runUnittest(double a, double b, double c, KSolves solution_type, double x1,
                 double x2) {
    double x1_test = NAN, x2_test = NAN;
    KSolves solution_type_test = solveSquare(a, b, c, &x1_test, &x2_test);
    if (solution_type != solution_type_test) {
        return false;
    }
    switch (solution_type) {
    case ZERO_SOLVES:
        return true;
    case ONE_SOLVES:
        return fabs(x1 - x1_test) < EPS;
    case TWO_SOLVES:
        return fabs(x1 - x1_test) < EPS && fabs(x2 - x2_test) < EPS;
    case INF_SOLVES:
        return true;
    default:
        assert(false);
    }
}

// TODO: fix
void parseArgs(int argc, char const *const *argv) {
    for (int i = 1; i < argc; i++) {
        if (strncmp("--", argv[i], 2) == 0) {
            processFlagString(argv[i] + 2);
        } else if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                processFlagChar(argv[i][j]);
            }
        }
    }
}

void processFlagChar(const char fl) {
    switch (fl) {
    case 'q':
        QUIET = true;
        break;
    case 'h':
        puts("mnogo hochech");
        break;
    default:
        break;
    }
}

void processFlagString(const char *fl) {
    if (!strcmp("quiet", fl)) {
        QUIET = true;
    } else if (!strcmp("help", fl)) {
        puts("mnogo hochech");
    }
}
