#define TESTS
#define MYDEBUGARGS if(GLOBAL_DEBUG_PARSE)
#define MYDEBUGPARSE if(GLOBAL_DEBUG_PARSE)

#include "main.h"
bool GLOBAL_QUIET = false;

bool GLOBAL_DEBUG_ARGS = false;
bool GLOBAL_DEBUG_PARSE = false;

int main(const int argc, const char *const *argv) {
    runUnittests();
    uint64_t flags = parseArgs(argc, argv);
    GLOBAL_QUIET = flags & (1 << FLAG_QUIET);
    GLOBAL_DEBUG_ARGS = flags & (1 << FLAG_DEBUGARGS);
    GLOBAL_DEBUG_PARSE = flags & (1 << FLAG_DEBUGPARSE);
    if (flags & (1 << FLAG_TESTFAILED)) {
        return 67;
    }
    if (flags & (1 << FLAG_SINGUP)) {
        singup();
        return 0;
    }
    if (!GLOBAL_QUIET) {
        printf("This not AI-generated reshalka\n");
        coloredPrintf(WHITE, "POL");
        coloredPrintf(BLUE, "TO");
        coloredPrintf(RED, "RASHKA\n");
    }
    if (!singin()) return 0;
    double a = NAN, b = NAN, c = NAN;
    while (!getKoefs(&a, &b, &c, flags & (1 << FLAG_TYPEENTER))) {
        double x1 = NAN, x2 = NAN;
        KSolves solution_type = solveSquare(a, b, c, &x1, &x2);
        printSolves(solution_type, x1, x2);
    }
    if (!GLOBAL_QUIET)
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

bool getKoefs(double *a, double *b, double *c, bool typeenter) {
    if (typeenter) {
        return getKoefsOld(a, b, c);
    }
    return getKoefsNew(a, b, c);
}

bool getKoefsOld(double *a, double *b, double *c) {
    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    assert(a != b);
    assert(b != c);
    assert(a != c);
    if (!GLOBAL_QUIET)
        printf("(old) enter a b c\n>>> ");
    int ch = 0;
    while (scanf("%lg %lg %lg", a, b, c) != 3) {
        while ((ch = getchar()) != '\n') {
            if (ch == 'q' || ch == EOF)
                return 1;
        }
        if (!GLOBAL_QUIET)
            printf("NO. enter a b c\n>>> ");
    }
    clearInputBuffer();
    return 0;
}

bool getKoefsNew(double *a, double *b, double *c) {
    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    assert(a != b);
    assert(b != c);
    assert(a != c); 
    char s_input[MAX_LEN] = {0};
    char s[MAX_LEN * 2] = {0};
    do {
        if (!GLOBAL_QUIET)
            printf("enter a eq\n>>> ");
        fgets(s_input, MAX_LEN, stdin);
        if (strchr(s_input, 'q') != NULL) return 1;
        int j = 0;
        for (int i = 0; s_input[i] != '\0'; i++) {
            if (s_input[i] != ' ' && s_input[i] != '\n') {
                if (s_input[i] == '-' || s_input[i] == '=') { 
                    s[j] = '+';
                    j++;
                }
                s[j] = s_input[i];
                j++;
            }
        }
        s[j] = '+';
    } while (parseKoefs(s, a, b, c));
    printf("START SOLVING...\n");
    return 0;
}

bool parseKoefs(char *s, double *a, double *b, double *c) {
    MYDEBUGPARSE printf("get: %s\n\n", s);
    double a_temp = 0, b_temp = 0, c_temp = 0, temp = 0;
    int8_t mn = 1;
    do {
        char* temp_s;
        temp = strtod(s, &temp_s);
        if (s == temp_s) {
            temp = 1;
        }
        s = temp_s;
        MYDEBUGPARSE printf("s: %s, temp: %lg, mn: %d\n", s, temp, mn);
        if (!strncmp(s, "x^2+", 4)) {
            a_temp += temp * mn;
            s += 4;
        } else if (!strncmp(s, "x+", 2)) {
            b_temp += temp * mn;
            s += 2;
        } else if (!strncmp(s, "+", 1)) {
            c_temp += temp * mn;
            s++;
        } else { 
            MYDEBUGPARSE printf("a b c: %lg %lg %lg\n", *a, *b, *c);
            return 1;
        }
        if (*s == '=') {
            if (mn < 0) return 1;
            s++;
            mn *= -1;
        }
    } while (*s != '\0');
    *a = a_temp;
    *b = b_temp;
    *c = c_temp;
    MYDEBUGPARSE printf("a b c: %lg %lg %lg\n", *a, *b, *c);
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
        if (!GLOBAL_QUIET) {
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

uint64_t parseArgs(int argc, char const *const *argv) {
    uint64_t flags = 0;
    for (int i = 1; i < argc; i++) {
        if (strncmp("--", argv[i], 2) == 0) {
            const char *args[MAX_ARGS_PER_FLAG] = {0};
            int argcc = 1;
            args[0] = argv[i];
            for (int j = i + 1; j < argc; j++) {
                if (argv[j][0] != '-') {
                    args[argcc] = argv[j];
                    argcc++;
                } else {
                    break;
                }
            }
            processFlagString(argcc, args, &flags);
            i += argcc - 1;
        }
    }
    return flags;
}

void processFlagString(const int argc, char const *const *argv, uint64_t *flags) {
    MYDEBUGARGS {
        printf("\n\n%s\nargs = %d\nargs = ", argv[0], argc);
        for (int i = 1; i < argc; i++) {
            printf("%s ", argv[i]);
        }
        putchar('\n');
    }
    if (!strcmp("--quiet", argv[0])) {
        *flags |= (1 << FLAG_QUIET);
    } else if (!strcmp("--help", argv[0])) {
        puts("mnogo hochech");
    } else if (!strcmp("--oldenter", argv[0])) {
        *flags |= (1 << FLAG_TYPEENTER);
    } else if (!strcmp("--testin", argv[0])) {
        if (argc == 2) {
            if (!runUnittestsFromFile(argv[1]))
                *flags |= (1 << FLAG_TESTFAILED);
        }
    } else if (!strcmp("--debugargs", argv[0])) {
        *flags |= (1 << FLAG_DEBUGARGS);
    } else if (!strcmp("--debugparse", argv[0])) {
        *flags |= (1 << FLAG_DEBUGPARSE);
    } else if (!strcmp("--singup", argv[0])) {
        *flags |= (1 << FLAG_SINGUP);
    }
}

bool runUnittestsFromFile(const char *filename) {
    assert(filename != NULL);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        coloredPrintf(RED, "FILE DOESN'T EXIST\n");
        return false;
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
    return !failed;
}

