#include "myassert.h"
#include <stdio.h>
#include <string.h>
#define TESTS
// #define MYDEBUGARGS

#include "main.h"
bool QUIET = false;

int main(const int argc, const char *const *argv) {
    runUnittests();
    parseArgs(argc, argv);
    if (!QUIET) {
        puts("This not AI-generated reshalka");
    }

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
    int ch = '\0';
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

bool isZero(double n) { return fabs(n) < EPS; }

bool isEqual(double a, double b) { return isZero(a - b); }

bool getKoefs(double *a, double *b, double *c) {
    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    if (!QUIET)
        printf("enter a b c\n>>> ");
    int ch = 0;
    while (scanf("%lg %lg %lg", a, b, c) != 3) {
        while ((ch = getchar()) != '\n') {
            if (ch == 'q' || ch == EOF)
                return 1;
        }
        if (!QUIET)
            printf("NO. enter a b c\n>>> ");
    }
    clearInputBuffer();
    return 0;
}

KSolves solveLinear(double k, double b, double *x) {
    if (isZero(k)) {
        if (isZero(b))
            return INF_SOLVES;
        return ZERO_SOLVES;
    }
    *x = -b / k;
    return ONE_SOLVE;
}

KSolves solveSquare(double a, double b, double c, double *x1, double *x2) {
    assert(isfinite(a));
    assert(isfinite(b));
    assert(isfinite(c));
    assert(x1 != NULL);
    assert(x2 != NULL);
    assert(x1 != x2);

    if (!isfinite(a) || !isfinite(b) || !isfinite(c)) {
        if (!QUIET) {
            printf("GET ISFINITE\n");
        }
        return ZERO_SOLVES;
    }

    if (isZero(a)) {
        return solveLinear(b, c, x1);
    }

    double D = b * b - 4 * a * c;
    if (D < -EPS) {
        return ZERO_SOLVES;
    } else if (D < EPS) {
        *x1 = -b / 2 / a;
        *x2 = *x1;
        return ONE_SOLVE;
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
    case ONE_SOLVE:
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
    case ONE_SOLVE:
        return isEqual(x1, x1_test);
    case TWO_SOLVES:
        return isEqual(x1, x1_test) && isEqual(x2, x2_test);
    case INF_SOLVES:
        return true;
    default:
        assert(false);
    }
}

void runUnittests() {
#ifdef TESTS
    assert(runUnittest(0, 0, 0, INF_SOLVES, 0, 0));
    assert(runUnittest(4, 5, 6, ZERO_SOLVES, 0, 0));
    assert(runUnittest(1, 2, 1, ONE_SOLVE, -1, 0));
    assert(runUnittest(1, 3, 1, TWO_SOLVES, -2.61803, -0.381966));
    assert(runUnittest(34, 444, 34, TWO_SOLVES, -12.9818, -0.077031));
    assert(runUnittest(7, 878, 7, TWO_SOLVES, -125.421, -0.00797317));
#endif
}

void parseArgs(int argc, char const *const *argv) {
    for (int i = 1; i < argc; i++) {
        if (strncmp("--", argv[i], 2) == 0) {
            const char *args[MAX_ARGS_PER_FLAG];
            int argcc = 1;
            args[0] = argv[i] + 2;
            for (int j = i + 1; j < argc; j++) {
                if (argv[j][0] != '-') {
                    args[argcc] = argv[j];
                    argcc++;
                } else {
                    break;
                }
            }
            processFlagString(argcc, args);
            i += argcc - 1;
        }
    }
}

void processFlagString(const int argc, char const *const *argv) {
#ifdef MYDEBUGARGS
    printf("\n\n%s\nargs = %d\nargs = ", argv[0], argc);
    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    putchar('\n');
#endif
    if (!strcmp("quiet", argv[0])) {
        QUIET = true;
    } else if (!strcmp("help", argv[0])) {
        puts("mnogo hochech");
    } else if (!strcmp("testin", argv[0])) {
        if (argc == 2) {
            runUnittestsFromFile(argv[1]);
        }
    }
}

void runUnittestsFromFile(const char *filename) {
    myassert(1 != 1);
    FILE *file = fopen(filename, "r");
    assert(file != NULL);
    if (file == NULL) {
        coloredPrintf(RED, "FILE DON'T EXISTS\n");
        return;
    }
    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN;
    KSolves solution_type = ZERO_SOLVES;
    char ch = 0;
    bool ans = false;
    bool failed = false;
    printf("starting tests...\n");
    while (fscanf(file, "%lg %lg %lg %c", &a, &b, &c, &ch) == 4) {
        switch (ch) {
        case '0':
            solution_type = ZERO_SOLVES;
            break;
        case '1':
            fscanf(file, "%lg", &x1);
            solution_type = ONE_SOLVE;
            break;
        case '2':
            fscanf(file, "%lg %lg", &x1, &x2);
            solution_type = TWO_SOLVES;
            break;
        default:
            solution_type = INF_SOLVES;
            break;
        }
        ans = runUnittest(a, b, c, solution_type, x1, x2);
        if (!ans) {
            coloredPrintf(RED, "FAILED: ");
            printf("%lg %lg %lg solution_type: %d solves: %lg %lg\n", a, b, c,
                   solution_type, x1, x2);
            failed = true;
        }
    }
    if (!failed) {
        coloredPrintf(GREEN, "Tests complete!\n");
    }
}
