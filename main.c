#define TESTS
#define MYDEBUGARGS if (GLOBAL_DEBUG_PARSE)
#define MYDEBUGPARSE if (GLOBAL_DEBUG_PARSE)

#include "main.h"
bool GLOBAL_QUIET = false;

bool GLOBAL_DEBUG_ARGS = false;
bool GLOBAL_DEBUG_PARSE = false;

int main(const int argc, const char *const *argv) {
    runUnittests();
    ArgValues values = parseArgs(argc, argv);
    GLOBAL_QUIET = values.flags & FLAG_QUIET;
    GLOBAL_DEBUG_ARGS = values.flags & FLAG_DEBUGARGS;
    GLOBAL_DEBUG_PARSE = values.flags & FLAG_DEBUGPARSE;
    if (values.flags & FLAG_SETSEED) {
        srand(values.seed);
    }
    if (values.flags & FLAG_RUNTEST) {
        if (!runUnittestsFromFile(values.filename)) return MAINERRORS_FAILEDTESTS;
    }
    if (values.flags & FLAG_RUNRANDOMTEST) {
        if (!runRandomUnittest(values.num_tests)) return MAINERRORS_FAILEDRANDOMTEST;
    }
    if (values.flags & FLAG_SIGNUP) {
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
    while (!getKoefs(&a, &b, &c, values.flags & FLAG_TYPEENTER)) {
        double x1 = NAN, x2 = NAN;
        KSolves solution_type = solveSquare(a, b, c, &x1, &x2);
        printSolves(solution_type, x1, x2);
        if (values.flags & FLAG_DRAWPLOT) {
            drawPlot(a, b, c);
        } else if (values.flags & FLAG_DRAWPLOTOFFSET) {
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
    char s_result[MAX_LEN * 2] = {0};
    int result_index = 1;
    do {
        QUIET printf("enter a eq\n>>> ");
        fgets(s_input, MAX_LEN, stdin);
        if (strchr(s_input, 'q') != NULL)
            return true;
        result_index = 0;
        for (int input_index = 0; s_input[input_index] != '\0'; input_index++) {
            if (s_input[input_index] != ' ' && s_input[input_index] != '\n') {
                if (s_input[input_index] == '-' || s_input[input_index] == '=') {
                    s_result[result_index] = '+';
                    result_index++;
                }
                s_result[result_index] = s_input[input_index];
                result_index++;
            }
        }
        s_result[result_index] = '+';
} while (result_index == 0 || parseKoefs(s_result, a, b, c) ||
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
    double a_temp = 0, b_temp = 0, c_temp = 0;
    int8_t sign = 1;
    static const char square_plus[] = "x^2+";
    static const char x_plus[] = "x+";
    static const char plus[] = "+";
    static const char minus_x_plus[] = "-x+";
    static const char minus_square_plus[] = "-x^2+";
    do {
        char *parse_end = NULL;
        double num_now = strtod(s, &parse_end);
        if (s == parse_end) {
            num_now = 1;
        }
        s = parse_end;
        MYDEBUGPARSE printf("s: %s, temp: %lg, mn: %d, a b c: %lg %lg %lg\n", s, num_now, sign, a_temp, b_temp, c_temp);
        if (!strncmp(s, square_plus, STATIC_STRLEN(square_plus))) {
            a_temp += num_now * sign; //TODO:  a_sum, s = s_input, 
            s += STATIC_STRLEN(square_plus);
        } else if (!strncmp(s, x_plus, STATIC_STRLEN(x_plus))) {
            b_temp += num_now * sign;
            s += STATIC_STRLEN(x_plus);
        } else if (!strncmp(s, plus, STATIC_STRLEN(plus))) {
            c_temp += num_now * sign;
            s += STATIC_STRLEN(plus);
        } else if (!strncmp(s, minus_x_plus, STATIC_STRLEN(minus_x_plus))) {
            b_temp -= num_now * sign;
            s += STATIC_STRLEN(minus_x_plus);
        } else if (!strncmp(s, minus_square_plus, STATIC_STRLEN(minus_square_plus))) {
            a_temp -= num_now * sign;
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

bool runUnittest(double a, double b, double c) {
    double x1 = NAN, x2 = NAN;
    KSolves solution_type = solveSquare(a, b, c, &x1, &x2);
    if (!checkTestAnswer(a, b, c, solution_type, x1, x2)) {
        QUIET printf("FAILED solve: a: %lg b: %lg c: %lg, x1: %lg x2: %lg\n",
                         a, b, c, x1, x2);
        return false;
    }
    return true;
}

void runUnittests() {
#ifdef TESTS
    assert(runUnittest(0, 0, 0));
    assert(runUnittest(4, 5, 6));
    assert(runUnittest(1, 2, 1));
    assert(runUnittest(2, -24, 64));
#endif
}

ArgValues parseArgs(int argc, char const *const *argv) {
    assert(argv != NULL);
    ArgValues ans = {.filename=NULL, .num_tests=0, .seed=42, .flags=0};
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
            processFlagString(argcc, args, &ans);
            i += argcc - 1;
        }
    }
    return ans;
}

void processFlagString(const int argc, char const *const *argv, ArgValues* values) {
    assert(argv != NULL);
    assert(values != NULL);
    MYDEBUGARGS {
        printf("\n\n%s\nargs = %d\nargs = ", argv[0], argc);
        for (int i = 1; i < argc; i++) {
            printf("%s ", argv[i]);
        }
        putchar('\n');
    }
    if (!strcmp("--quiet", argv[0])) {
        values->flags |= (1 << FLAG_QUIET);
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
               "--randtests [num_tests]     generate random tests\n"
               "--seed [seed]               set seed\n");
    } else if (!strcmp("--oldenter", argv[0])) {
        values->flags |= FLAG_TYPEENTER;
    } else if (!strcmp("--testin", argv[0])) {
        if (argc == 2) {
            values->flags |= FLAG_RUNTEST;
            values->filename = argv[1];
        }
    } else if (!strcmp("--debugargs", argv[0])) {
        values->flags |= FLAG_DEBUGARGS;
    } else if (!strcmp("--debugparse", argv[0])) {
        values->flags |= FLAG_DEBUGPARSE;
    } else if (!strcmp("--drawplot", argv[0])) {
        values->flags |= FLAG_DRAWPLOT;
    } else if (!strcmp("--drawplotoffset", argv[0])) {
        values->flags |= FLAG_DRAWPLOTOFFSET;
    } else if (!strcmp("--signup", argv[0])) {
        values->flags |= FLAG_SIGNUP;
    } else if (!strcmp("--randtests", argv[0])) {
        if (argc == 2) {
            values->flags |= FLAG_RUNRANDOMTEST;
            values->num_tests = strtol(argv[1], NULL, 10);
        }
    } else if (!strcmp("--seed", argv[0])) {
        if (argc == 2) {
            values->flags |= FLAG_SETSEED;
            values->seed = (uint32_t)strtol(argv[1], NULL, 10);
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
    double a = NAN, b = NAN, c = NAN;
    QUIET printf("starting tests from file...\n");
    while (fscanf(file, "%lg %lg %lg", &a, &b, &c) == 3) {
        bool ans = runUnittest(a, b, c);
        if (!ans) {
            return false;
        }
    } 
    fclose(file);
    QUIET coloredPrintf(GREEN, "TEST FROM FILE PASSED\n");
    return true;
}

bool runRandomUnittest(long num_tests) {
    QUIET printf("starting tests from file...\n");
    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN, x1_ref = NAN, x2_ref = NAN;
    for (int i = 0; i < num_tests; i++) {
        a = randDouble();
        b = randDouble();
        c = randDouble();
        if (!runUnittest(a, b, c)) {
            return false;
        }
    }
    for (int i = 0; i < num_tests; i++) {
        b = randDouble();
        c = randDouble();
        if (!runUnittest(0, b, c)) {
            return false;
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
        KSolves solution_type = solveSquare(a, b, c, &x1, &x2);
        if (!checkTestAnswer(a, b, c, solution_type, x1, x2)) {
            return false;
        }
    }
    QUIET coloredPrintf(GREEN, "RANDOM TEST PASSED\n");
    return true;
}

double countEq(double a, double b, double c, double x) {
    return a * x * x + b * x + c;
}

bool checkTestAnswer(double a, double b, double c, KSolves solution_type_ans, double x1, double x2) {
    KSolves solution_type_ref = getRightSolutionType(a, b, c);
    if (solution_type_ans != solution_type_ref) return false;
    switch (solution_type_ans) {
        case ZERO_SOLVES:
        case INF_SOLVES:
            return true;
        case ONE_SOLVE:
            return isZero(countEq(a, b, c, x1));
        case TWO_SOLVES:
            return isZero(countEq(a, b, c, x1)) && isZero(countEq(a, b, c, x2));
        default:
            assert(false);
    }
}

KSolves getRightSolutionType(double a, double b, double c) {
    if (!isZero(a)) {
        double x0 = -b / (2 * a);
        double y0 = countEq(a, b, c, x0);
        if (y0 < -EPS) {
            return TWO_SOLVES;
        } else if (y0 < EPS) {
            return ONE_SOLVE;
        } else {
            return ZERO_SOLVES;
        }
    } else if (!isZero(b)) {
        return ONE_SOLVE;
    } else if (!isZero(c)) {
        return ZERO_SOLVES;
    } else {
        return INF_SOLVES;
    }
}

void drawPlotOffset(double a, double b, double c) {
    assert(isfinite(a));
    assert(isfinite(b));
    assert(isfinite(c));
    char plot[SIZE_PLOT_Y][SIZE_PLOT_X] = {}; 
    memset(plot, ' ', sizeof plot);
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
        y = (countEq(a, b, c, ((i - SIZE_PLOT_X / 2) - offsetx) * stepx) + offsety) * stepy;
        if (y < SIZE_PLOT_Y / 2 && y > -SIZE_PLOT_Y / 2)
            plot[(int)(SIZE_PLOT_Y / 2 - y)][i] = '*';
    }

    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        printf("%.*s\n", SIZE_PLOT_X, plot[i]);
    }
}

void drawPlot(double a, double b, double c) {
    assert(isfinite(a));
    assert(isfinite(b));
    assert(isfinite(c));
    char plot[SIZE_PLOT_Y][SIZE_PLOT_X] = {}; 
    memset(plot, ' ', sizeof plot);
    for (int i = 0; i < SIZE_PLOT_X; i++) {
        plot[SIZE_PLOT_Y / 2][i] = '-';
    }
    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        plot[i][SIZE_PLOT_X / 2] = '|';
    }
    plot[SIZE_PLOT_Y / 2][SIZE_PLOT_X / 2] = '+';

    double stepx = 0.2, stepy = 0.4;
    if (isZero(a)) {
        if (fabs(c) * stepy > SIZE_PLOT_Y / 4 * 3) {
            double scale = SIZE_PLOT_Y / 4 * 3 / c;
            stepx *= scale;
            stepy *= scale;
        }
    } else {
        double x0 = -b / (2 * a);
        double y0 = countEq(a, b, c, x0);
        while (fabs(x0) * stepx > SIZE_PLOT_X / 4 ||
               fabs(y0) * stepy > SIZE_PLOT_Y / 4) {
            stepx /= 2;
            stepy /= 2;
        }
    }

    double y = NAN;
    for (int i = 0; i < SIZE_PLOT_X; i++) {
        y = countEq(a, b, c, (i - SIZE_PLOT_X / 2) * stepx) * stepy;
        if (y < SIZE_PLOT_Y / 2 && y > -SIZE_PLOT_Y / 2)
            plot[(int)(SIZE_PLOT_Y / 2 - y)][i] = '*';
    }

    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        printf("%.*s\n", SIZE_PLOT_X, plot[i]);
    }
}
