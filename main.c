#define TESTS
#define MYDEBUGARGS if (GLOBAL_DEBUG_PARSE)
#define MYDEBUGPARSE if (GLOBAL_DEBUG_PARSE)

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
        return MAINERRORS_FAILEDTESTS;
    }
    if (flags & (1 << FLAG_RANDOMTESTFAILED)) {
        return MAINERRORS_FAILEDRANDOMTEST;
    }
    if (flags & (1 << FLAG_SIGNUP)) {
        signup();
        return 0;
    }
    QUIET {
        printf("This not AI-generated reshalka\n");
        coloredPrintf(WHITE, "POL");
        coloredPrintf(BLUE, "TO");
        coloredPrintf(RED, "RASHKA\n");
    }
    if (!signin())
        return 0;
    double a = NAN, b = NAN, c = NAN;
    while (!getKoefs(&a, &b, &c, flags & (1 << FLAG_TYPEENTER))) {
        double x1 = NAN, x2 = NAN;
        KSolves solution_type = solveSquare(a, b, c, &x1, &x2);
        printSolves(solution_type, x1, x2);
        if (flags & (1 << FLAG_DRAWPLOT)) {
            drawPlot(a, b, c);
        } else if (flags & (1 << FLAG_DRAWPLOTOFFSET)) {
            drawPlotOffset(a, b, c);
        }
    }
    QUIET printf("Bye bye! ;)\n");
    return 0;
}

bool getKoefs(double *a, double *b, double *c, bool typeenter) {
    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
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
    QUIET printf("(old) enter a b c\n>>> ");
    int ch = 0;
    while (scanf("%lg %lg %lg", a, b, c) != 3) {
        while ((ch = getchar()) != '\n') {
            if (ch == 'q' || ch == EOF)
                return 1;
        }
        QUIET printf("NO. enter a b c\n>>> ");
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
    int j = 1;
    do {
        QUIET printf("enter a eq\n>>> ");
        fgets(s_input, MAX_LEN, stdin);
        if (strchr(s_input, 'q') != NULL)
            return true;
        j = 0;
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
    } while (j == 0 || parseKoefs(s, a, b, c) ||
             !(isfinite(*a) && isfinite(*b) && isfinite(*c)));
    printf("START SOLVING...\n");
    return false;
}

bool parseKoefs(char *s, double *a, double *b, double *c) {
    assert(s != NULL);
    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    MYDEBUGPARSE printf("get: %s\n\n", s);
    double a_temp = 0, b_temp = 0, c_temp = 0, temp = 0;
    int8_t sign = 1;
    static const char square_plus[] = "x^2+";
    static const char x_plus[] = "x+";
    static const char plus[] = "+";
    static const char minus_x_plus[] = "-x+";
    static const char minus_square_plus[] = "-x^2+";
    do {
        char *temp_s;
        temp = strtod(s, &temp_s);
        if (s == temp_s) {
            temp = 1;
        }
        s = temp_s;
        MYDEBUGPARSE printf("s: %s, temp: %lg, mn: %d, a b c: %lg %lg %lg\n", s, temp, sign, a_temp, b_temp, c_temp);
        if (!strncmp(s, square_plus, STATIC_STRLEN(square_plus))) {
            a_temp += temp * sign;
            s += STATIC_STRLEN(square_plus);
        } else if (!strncmp(s, x_plus, STATIC_STRLEN(x_plus))) {
            b_temp += temp * sign;
            s += STATIC_STRLEN(x_plus);
        } else if (!strncmp(s, plus, STATIC_STRLEN(plus))) {
            c_temp += temp * sign;
            s += STATIC_STRLEN(plus);
        } else if (!strncmp(s, minus_x_plus, STATIC_STRLEN(minus_x_plus))) {
            b_temp -= temp * sign;
            s += STATIC_STRLEN(minus_x_plus);
        } else if (!strncmp(s, minus_square_plus, STATIC_STRLEN(minus_square_plus))) {
            a_temp -= temp * sign;
            s += STATIC_STRLEN(minus_square_plus);
        } else if (s[0] != '\0') {
            MYDEBUGPARSE printf("s: %s, a b c: %lg %lg %lg\n", s, *a, *b, *c);
            return 1;
        }
        if (*s == '=') {
            if (sign < 0)
                return 1;
            s++;
            sign *= -1;
            if (*s == '+')
                s++;
        }
    } while (*s != '\0');
    *a = a_temp;
    *b = b_temp;
    *c = c_temp;
    MYDEBUGPARSE printf("a b c: %lg %lg %lg\n", *a, *b, *c);
    return 0;
}

KSolves solveLinear(double k, double b, double *x) {
    assert(isfinite(k));
    assert(isfinite(b));
    assert(x != NULL);
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
        puts("NO solutions");
        break;
    case ONE_SOLVE:
        printf("one solution: %lg -\n", x1);
        break;
    case TWO_SOLVES:
        printf("two solutions: %lg %lg\n", x1, x2);
        break;
    case INF_SOLVES:
        puts("ALL IN");
        break;
    default:
        assert(false);
    }
}

bool runUnittest(double a, double b, double c, KSolves solution_type, double x1, double x2) {
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
        return (isEqual(x1, x1_test) && isEqual(x2, x2_test)) ||
               (isEqual(x2, x1_test) && isEqual(x1, x2_test));
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
    assert(runUnittest(2, -24, 64, TWO_SOLVES, 4, 8));
#endif
}

uint64_t parseArgs(int argc, char const *const *argv) {
    assert(argv != NULL);
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
    assert(argv != NULL);
    assert(flags != NULL);
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
        printf("Usage:\n"
               "--quiet                     disable dialog outputs\n"
               "--oldenter                  enable old enter (a b c)\n"
               "--testin [file.txt]         unittests from file\n"
               "--debugargs                 debug argparse\n"
               "--debugparse                debug parsing eq\n"
               "--drawplot                  draw a plot\n"
               "--drawplotoffset            draw a plot by offsets\n"
               "--signup                    register a user\n"
               "--randtests [num_tests]      generate random tests\n"
               "--seed [seed]               set seed\n");
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
    } else if (!strcmp("--drawplot", argv[0])) {
        *flags |= (1 << FLAG_DRAWPLOT);
    } else if (!strcmp("--drawplotoffset", argv[0])) {
        *flags |= (1 << FLAG_DRAWPLOTOFFSET);
    } else if (!strcmp("--signup", argv[0])) {
        *flags |= (1 << FLAG_SIGNUP);
    } else if (!strcmp("--randtests", argv[0])) {
        if (argc == 2) {
            if (!runRandomUnittest(strtol(argv[1], NULL, 10))) {
                *flags |= (1 << FLAG_RANDOMTESTFAILED);
            }
        }
    } else if (!strcmp("--seed", argv[0])) {
        if (argc == 2) {
            srand((uint32_t)strtol(argv[1], NULL, 10));
        }
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

bool runRandomUnittest(long num_tests) {
    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN, x1_ref = NAN, x2_ref = NAN;
    KSolves solution_type = ZERO_SOLVES, solution_type_ref = ZERO_SOLVES;
    double x0 = NAN, y0 = NAN;
    for (int i = 0; i < num_tests; i++) {
        a = randDouble();
        b = randDouble();
        c = randDouble();
        if (!isZero(a)) {
            x0 = -b / (2 * a);
            y0 = countEq(a, b, c, x0);
            if (y0 < -EPS) {
                solution_type_ref = TWO_SOLVES;
            } else if (y0 < EPS) {
                solution_type_ref = ONE_SOLVE;
            } else {
                solution_type_ref = ZERO_SOLVES;
            }
            solution_type = solveSquare(a, b, c, &x1, &x2);
            if (solution_type != solution_type_ref) {
                QUIET printf("FAILED solution_type: a b c: %lg %lg %lg\n", a, b, c);
                return false;
            }
            switch (solution_type) {
            case ZERO_SOLVES:
                break;
            case ONE_SOLVE:
                if (!isZero(countEq(a, b, c, x1))) {
                    QUIET printf("FAILED solve: a b c x: %lg %lg %lg %lg\n", a, b, c, x1);
                    return false;
                }
                break;
            case TWO_SOLVES:
                if (!isZero(countEq(a, b, c, x1)) || !isZero(countEq(a, b, c, x2))) {
                    QUIET printf("FAILED solve: a b c x1 x2: %lg %lg %lg %lg %lg\n", a, b, c, x1, x2);
                    return false;
                }
                break;
            case INF_SOLVES:
            default:
                assert(false);
            }
        }
    }
    a = 0;
    for (int i = 0; i < num_tests; i++) {
        b = randDouble();
        c = randDouble();
        if (!isZero(b)) {
            solution_type_ref = ONE_SOLVE;
            solution_type = solveSquare(a, b, c, &x1, &x2);
            if (solution_type != solution_type_ref) {
                QUIET printf("FAILED solution_type: a b c: %lg %lg %lg\n", a, b, c);
                return false;
            }
            if (!isZero(countEq(a, b, c, x1))) {
                QUIET printf("FAILED solve: a b c x: %lg %lg %lg %lg\n", a, b, c, x1);
                return false;
            }   
        }
    }
    for (int i = 0; i < num_tests; i++) {
        x1_ref = randDouble();
        x2_ref = randDouble();
        if (x1_ref > x2_ref) {
            double temp = x1_ref;
            x1_ref = x2_ref;
            x2_ref = temp;
        }
        a = randDouble();
        b = -a * (x1_ref + x2_ref);
        c = x1_ref * x2_ref * a;
        solution_type = solveSquare(a, b, c, &x1, &x2);
        if (solution_type != TWO_SOLVES || !isEqual(x1, x1_ref) || !isEqual(x2, x2_ref)) {
            QUIET printf("FAILED solve: a b c: %lg %lg %lg x1: %lg x2: %lg x1_ref: %lg x2_ref: %lg\n", a, b, c, x1, x2, x1_ref, x2_ref);
            return false;
        }   
    }
    QUIET printf("TEST PASSED\n");
    return true;
}

double countEq(double a, double b, double c, double x) {
    return a * x * x + b * x + c;
}

void drawPlotOffset(double a, double b, double c) {
    assert(isfinite(a));
    assert(isfinite(b));
    assert(isfinite(c)); 
    char plot[SIZE_PLOT_Y][SIZE_PLOT_X+1] = {};
    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        for (int j = 0; j < SIZE_PLOT_X; j++) {
            plot[i][j] = ' ';
        }
    }
    for (int i = 0; i < SIZE_PLOT_X; i++) {
        plot[SIZE_PLOT_Y / 2][i] = '-';
    }
    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        plot[i][SIZE_PLOT_X / 2] = '|';
    }
    plot[SIZE_PLOT_Y / 2][SIZE_PLOT_X / 2] = '+';

    double stepx = 0.2, stepy = 0.4, offsetx = 0, offsety = 0;
    if (isZero(a)) {
        offsety = -c;
    } else {
        offsetx = b / (2 * a);
        offsety = -countEq(a, b, c, -offsetx);
    }

    double y = NAN;
    for (int i = 0; i < SIZE_PLOT_X; i++) {
        y = (countEq(a, b, c, (i - SIZE_PLOT_X / 2) * stepx - offsetx) + offsety) * stepy;
        if (y < SIZE_PLOT_Y / 2 && y > -SIZE_PLOT_Y / 2) plot[(int)(SIZE_PLOT_Y / 2 - y)][i] = '*';
    }

    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        printf("%s\n", plot[i]);
    }
}

void drawPlot(double a, double b, double c) {
    assert(isfinite(a));
    assert(isfinite(b));
    assert(isfinite(c)); 
    char plot[SIZE_PLOT_Y][SIZE_PLOT_X+1] = {};
    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        for (int j = 0; j < SIZE_PLOT_X; j++) {
            plot[i][j] = ' ';
        }
    }
    for (int i = 0; i < SIZE_PLOT_X; i++) {
        plot[SIZE_PLOT_Y / 2][i] = '-';
    }
    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        plot[i][SIZE_PLOT_X / 2] = '|';
    }
    plot[SIZE_PLOT_Y / 2][SIZE_PLOT_X / 2] = '+';

    double stepx = 0.1, stepy = 0.2;
    if (isZero(a)) {
        if (fabs(c) * stepy > SIZE_PLOT_Y / 4 * 3) {
            double scale = SIZE_PLOT_Y / 4 * 3 / c;
            stepx *= scale;
            stepy *= scale;
        }
    } else {
        double x0 = -b / (2 * a);
        double y0 = countEq(a, b, c, x0);
        while (fabs(x0) * stepx > SIZE_PLOT_X / 4 || fabs(y0) * stepy > SIZE_PLOT_Y / 4) {
            stepx /= 2;
            stepy /= 2;
        } 
    }

    double y = NAN;
    for (int i = 0; i < SIZE_PLOT_X; i++) {
        y = countEq(a, b, c, (i - SIZE_PLOT_X / 2) * stepx) * stepy;
        if (y < SIZE_PLOT_Y / 2 && y > -SIZE_PLOT_Y / 2) plot[(int)(SIZE_PLOT_Y / 2 - y)][i] = '*';
    }

    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        printf("%s\n", plot[i]);
    }
}
