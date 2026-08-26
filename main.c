#define TESTS
#define MYDEBUGARGS if (GLOBAL_DEBUG_PARSE)
#define MYDEBUGPARSE if (GLOBAL_DEBUG_PARSE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <curses.h>
#include <math.h>

#include "colors.h"
#include "hash.h"
#include "common.h"
#include "main.h"

bool GLOBAL_QUIET = false;
bool GLOBAL_DEBUG_ARGS = false;
bool GLOBAL_DEBUG_PARSE = false;

int main(const int argc, const char *argv[]) {
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
        if (!runRandomTest(values.num_tests)) return MAINERRORS_FAILEDRANDOMTEST;
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
        return MAINERRORS_FAILEDSIGNIN;
    SquareKoefs koefs = {NAN, NAN, NAN};
    while (!getKoefs(&koefs, values.flags & FLAG_TYPEENTER)) {
        double x1 = NAN, x2 = NAN;
        KSolves solution_type = solveSquare(koefs, &x1, &x2);
        printSolves(solution_type, x1, x2);
        if (values.flags & FLAG_DRAWPLOT) {
            drawPlot(koefs);
        } else if (values.flags & FLAG_DRAWPLOTOFFSET) {
            drawPlotOffset(koefs);
        }
    }
    QUIET printf("Bye bye! ;)\n");
    return 0;
}

bool getKoefs(SquareKoefs *koefs, bool typeenter) {
    assert(koefs != NULL);
    if (typeenter) {
        return getKoefsOld(koefs);
    }
    return getKoefsNew(koefs);
}

// [[deprecated("This is old method, use getKoefs")]]
bool getKoefsOld(SquareKoefs *koefs) {
    assert(koefs != NULL); 
    QUIET printf("(old) enter a b c\n>>> ");
    int ch = 0;
    while (scanf("%lg %lg %lg", &(koefs->a), &(koefs->b), &(koefs->c)) != 3) {
        while ((ch = getchar()) != '\n') {
            if (ch == 'q' || ch == EOF)
                return 1;
        }
        QUIET printf("NO. enter a b c\n>>> ");
    }
    clearInputBuffer();
    return 0;
}

bool getKoefsNew(SquareKoefs *koefs) {
    assert(koefs != NULL); 
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
    } while (result_index == 0 || parseKoefs(s_result, koefs) ||
        !(isfinite(koefs->a) && isfinite(koefs->b) && isfinite(koefs->c)));
    printf("START SOLVING...\n");
    return false;
}

bool parseKoefs(char *s_input, SquareKoefs *koefs) {
    assert(s_input != NULL);
    assert(koefs != NULL);
    MYDEBUGPARSE printf("get: %s\n\n", s_input);
    double a_sum = 0, b_sum = 0, c_sum = 0;
    int8_t sign = 1;
    static const char square_plus[] = "x^2+";
    static const char x_plus[] = "x+";
    static const char plus[] = "+";
    static const char minus_x_plus[] = "-x+";
    static const char minus_square_plus[] = "-x^2+";
    do {
        char *parse_end = NULL;
        double num_now = strtod(s_input, &parse_end);
        if (s_input == parse_end) {
            num_now = 1;
        }
        s_input = parse_end;
        MYDEBUGPARSE printf("s: %s, temp: %lg, mn: %d, a b c: %lg %lg %lg\n", s_input, num_now, sign, a_sum, b_sum, c_sum);
        if (!strncmp(s_input, square_plus, STATIC_STRLEN(square_plus))) {
            a_sum += num_now * sign;
            s_input += STATIC_STRLEN(square_plus);
        } else if (!strncmp(s_input, x_plus, STATIC_STRLEN(x_plus))) {
            b_sum += num_now * sign;
            s_input += STATIC_STRLEN(x_plus);
        } else if (!strncmp(s_input, plus, STATIC_STRLEN(plus))) {
            c_sum += num_now * sign;
            s_input += STATIC_STRLEN(plus);
        } else if (!strncmp(s_input, minus_x_plus, STATIC_STRLEN(minus_x_plus))) {
            b_sum -= num_now * sign;
            s_input += STATIC_STRLEN(minus_x_plus);
        } else if (!strncmp(s_input, minus_square_plus, STATIC_STRLEN(minus_square_plus))) {
            a_sum -= num_now * sign;
            s_input += STATIC_STRLEN(minus_square_plus);
        } else if (s_input[0] != '\0') {
            MYDEBUGPARSE printf("s: %s, a b c: %lg %lg %lg\n", s_input, koefs->a, koefs->b, koefs->c);
            return 1;
        }
        if (*s_input == '=') {
            if (sign < 0)
                return 1;
            s_input++;
            sign *= -1;
            if (*s_input == '+')
                s_input++;
        }
    } while (*s_input != '\0');
    koefs->a = a_sum;
    koefs->b = b_sum;
    koefs->c = c_sum;
    MYDEBUGPARSE printf("a b c: %lg %lg %lg\n", koefs->a, koefs->b, koefs->c);
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

KSolves solveSquare(SquareKoefs koefs, double *x1, double *x2) {
    assert(isfinite(koefs.a));
    assert(isfinite(koefs.b));
    assert(isfinite(koefs.c));
    assert(x1 != NULL);
    assert(x2 != NULL);
    assert(x1 != x2);

    if (isZero(koefs.a)) {
        return solveLinear(koefs.b, koefs.c, x1);
    }

    double D = koefs.b * koefs.b - 4 * koefs.a * koefs.c;
    if (D < -EPS) {
        return ZERO_SOLVES;
    } else if (D < EPS) {
        *x1 = -koefs.b / (2 * koefs.a);
        *x2 = *x1;
        return ONE_SOLVE;
    } else {
        double sqrtD = sqrt(D);
        *x1 = (-koefs.b - sqrtD) / (2 * koefs.a);
        *x2 = (-koefs.b + sqrtD) / (2 * koefs.a);
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

bool runTest(SquareKoefs koefs) {
    double x1 = NAN, x2 = NAN;
    KSolves solution_type = solveSquare(koefs, &x1, &x2);
    if (!checkTestAnswer(koefs, solution_type, x1, x2)) {
        QUIET printf("FAILED solve: a: %lg b: %lg c: %lg, x1: %lg x2: %lg\n",
                         koefs.a, koefs.b, koefs.c, x1, x2);
        return false;
    }
    return true;
}

bool runUnittest(TestCase test) {
    double x1 = NAN, x2 = NAN;
    bool passed = true;
    KSolves solution_type = solveSquare(test.koefs, &x1, &x2);
    if (solution_type != test.solution_type) {
        passed = false;
    } else {
        switch (solution_type) {
        case ZERO_SOLVES:
            passed = true;
            break;
        case ONE_SOLVE:
            passed = isEqual(test.x1, x1);
            break;
        case TWO_SOLVES:
            passed = (isEqual(test.x1, x1) && isEqual(test.x2, x2)) || (isEqual(test.x2, x1) && isEqual(test.x1, x2));\
            break;
        case INF_SOLVES:
            passed = true;
            break;
        default:
            assert(false);
        }
    }
    if (!passed) {
        QUIET printf("FAILED solve: a: %lg b: %lg c: %lg, x1: %lg x2: %lg, x1_ref: %lg, x2_ref: %lg, solution_type: %d, solution_type_ref: %d\n",
                         test.koefs.a, test.koefs.b, test.koefs.c, 
                         x1, x2, test.x1, test.x2, 
                         solution_type, test.solution_type);
    }
    return passed;

}

void runUnittests() {
#ifdef TESTS
    assert(runUnittest((TestCase){ {0,   0,   0},   INF_SOLVES        }));
    assert(runUnittest((TestCase){ {4,   5,   6},  ZERO_SOLVES        }));
    assert(runUnittest((TestCase){ {1,   2,   1},    ONE_SOLVE, -1    }));
    assert(runUnittest((TestCase){ {2, -24,  64},   TWO_SOLVES,  4, 8 }));
#endif
}

ArgValues parseArgs(int argc, char const *const *argv) {
    assert(argv != NULL);

    ArgValues ans = {.filename=NULL, .num_tests=0, .seed=42, .flags=0};

    int i = 1;
    while (strncmp("--", argv[i], 2)) i++;

    for (; i < argc; i++) {
        const char *argv_per_flag[MAX_ARGS_PER_FLAG] = {0};
        int argc_per_flag = 1;
        argv_per_flag[0] = argv[i];

        for (i++; i < argc; i++) {
            if (argv[i][0] != '-') {
                argv_per_flag[argc_per_flag] = argv[i];
                argc_per_flag++;
            } else {
                i--;
                break;
            }
        }
        processFlagString(argc_per_flag, argv_per_flag, &ans);
    }
    return ans;
}

static const myFlag FLAGS[] = {
    {"--quiet",          FLAG_QUIET,            NULL,    "disable dialog outputs"},
    {"--oldenter",       FLAG_TYPEENTER,        NULL,    "enable old enter (a b c)"},
    {"--testin",         FLAG_RUNTEST,          NULL,    "parametr: [file.txt] unittests from file"},
    {"--debugargs",      FLAG_DEBUGARGS,        NULL,    "debug argparse"},
    {"--debugparse",     FLAG_DEBUGPARSE,       NULL,    "debug parsing eq"},
    {"--drawplot",       FLAG_DRAWPLOT,         NULL,    "draw a plot"},
    {"--drawplotoffset", FLAG_DRAWPLOTOFFSET,   NULL,    "draw a plot by offsets"},
    {"--signup",         FLAG_SIGNUP,           NULL,    "register a user"},
    {"--randtests",      FLAG_RUNRANDOMTEST,    NULL,    "parametr: [num_tests] generate random tests"},
    {"--seed",           FLAG_SETSEED,          NULL,    "parametr: [seed] set seed"}
};

// void setSeed(const int argc, const char *argv[]) {
//     if (argc == 2) {
//         srand((uint32_t)strtol(argv[1], NULL, 10));
//     }
// }

void processFlagString(const int argc, char const *argv[], ArgValues* values) {
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
    SquareKoefs koefs = {};
    char solution_type_char = 0;
    bool passed = true;
    printf("starting tests...\n");
    while (fscanf(file, "%lg %lg %lg %c", &koefs.a, &koefs.b, &koefs.c, &solution_type_char) == 4) {
        KSolves solution_type_ref = ZERO_SOLVES;
        double x1 = NAN, x2 = NAN;
        switch (solution_type_char) {
        case '0':
            solution_type_ref = ZERO_SOLVES;
            break;
        case '1':
            fscanf(file, "%lg", &x1);
            solution_type_ref = ONE_SOLVE;
            break;
        case '2':
            fscanf(file, "%lg %lg", &x1, &x2);
            solution_type_ref = TWO_SOLVES;
            break;
        default:
            solution_type_ref = INF_SOLVES;
            break;
        }
        bool ans = runUnittest((TestCase) {(SquareKoefs) koefs, solution_type_ref, x1, x2});
        passed = passed && ans;
    }
    if (passed) {
        coloredPrintf(GREEN, "Tests complete!\n");
    }
    fclose(file);
    return passed;
}

bool runRandomTest(long num_tests) {
    QUIET printf("starting tests from file...\n");
    double x1 = NAN, x2 = NAN, x1_ref = NAN, x2_ref = NAN;
    SquareKoefs koefs = {NAN, NAN, NAN};
    for (int i = 0; i < num_tests; i++) {
        koefs.a = randDouble();
        koefs.b = randDouble();
        koefs.c = randDouble();
        if (!runTest(koefs)) {
            return false;
        }
    }
    koefs.a = 0;
    for (int i = 0; i < num_tests; i++) {
        koefs.b = randDouble();
        koefs.c = randDouble();
        if (!runTest(koefs)) {
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
        koefs.a = randDouble();
        koefs.b = -koefs.a * (x1_ref + x2_ref);
        koefs.c = x1_ref * x2_ref * koefs.a;
        KSolves solution_type = solveSquare(koefs, &x1, &x2);
        if (!checkTestAnswer(koefs, solution_type, x1, x2)) {
            return false;
        }
    }
    QUIET coloredPrintf(GREEN, "RANDOM TEST PASSED\n");
    return true;
}

double countEq(SquareKoefs koefs, double x) {
    return koefs.a * x * x + koefs.b * x + koefs.c;
}

bool checkTestAnswer(SquareKoefs koefs, KSolves solution_type_ans, double x1, double x2) {
    KSolves solution_type_ref = getRightSolutionType(koefs);
    if (solution_type_ans != solution_type_ref) return false;
    switch (solution_type_ans) {
        case ZERO_SOLVES:
        case INF_SOLVES:
            return true;
        case ONE_SOLVE:
            return isZero(countEq(koefs, x1));
        case TWO_SOLVES:
            return isZero(countEq(koefs, x1)) && isZero(countEq(koefs, x2));
        default:
            assert(false);
    }
}

KSolves getRightSolutionType(SquareKoefs koefs) {
    if (!isZero(koefs.a)) {
        double x0 = -koefs.b / (2 * koefs.a);
        double y0 = countEq(koefs, x0);
        if (y0 < -EPS) {
            return TWO_SOLVES;
        } else if (y0 < EPS) {
            return ONE_SOLVE;
        } else {
            return ZERO_SOLVES;
        }
    } else if (!isZero(koefs.b)) {
        return ONE_SOLVE;
    } else if (!isZero(koefs.c)) {
        return ZERO_SOLVES;
    } else {
        return INF_SOLVES;
    }
}

void drawPlotOffset(SquareKoefs koefs) {
    assert(isfinite(koefs.a));
    assert(isfinite(koefs.b));
    assert(isfinite(koefs.c));
    char plot[SIZE_PLOT_Y][SIZE_PLOT_X] = {}; 
    memset(plot, ' ', sizeof plot);
    for (int i = 0; i < SIZE_PLOT_X; i++) {
        plot[SIZE_PLOT_Y / 2][i] = '-';
    }
    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        plot[i][SIZE_PLOT_X / 2] = '|';
    }
    plot[SIZE_PLOT_Y / 2][SIZE_PLOT_X / 2] = '+';

    double stepx = 5, stepy = 0.4, offsetx = 0, offsety = 0;
    if (isZero(koefs.a)) {
        offsety = -koefs.c;
    } else {
        offsetx = koefs.b / (2 * koefs.a);
        offsety = -countEq(koefs, -offsetx);
    }

    double y = NAN;
    for (int i = -SIZE_PLOT_X / 2; i < SIZE_PLOT_X / 2; i++) {
        /* i - координата в точках
        * поделив на stepx, получаем обычные координаты
        * двигаем х, получаем ответ от функции, двигаем и переводим в точки
        */
        y = (countEq(koefs, i / stepx - offsetx) + offsety) * stepy;
        if (y < SIZE_PLOT_Y / 2 && y > -SIZE_PLOT_Y / 2)
            plot[(int)(SIZE_PLOT_Y / 2 - y)][i + SIZE_PLOT_X / 2] = '*';
    }
    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        printf("%.*s\n", SIZE_PLOT_X, plot[i]);
    }
}

void drawPlot(SquareKoefs koefs) {
    assert(isfinite(koefs.a));
    assert(isfinite(koefs.b));
    assert(isfinite(koefs.c));
    char plot[SIZE_PLOT_Y][SIZE_PLOT_X] = {}; 
    memset(plot, ' ', sizeof plot);
    for (int i = 0; i < SIZE_PLOT_X; i++) {
        plot[SIZE_PLOT_Y / 2][i] = '-';
    }
    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        plot[i][SIZE_PLOT_X / 2] = '|';
    }
    plot[SIZE_PLOT_Y / 2][SIZE_PLOT_X / 2] = '+';

    double stepx = 5, stepy = 0.4;
    if (isZero(koefs.a)) {
        if (fabs(koefs.c) * stepy > SIZE_PLOT_Y / 4 * 3) {
            double scale = SIZE_PLOT_Y / 4 * 3 / koefs.c;
            stepx *= scale;
            stepy *= scale;
        }
    } else {
        double x0 = -koefs.b / (2 * koefs.a);
        double y0 = countEq(koefs, x0);
        while (fabs(x0) * stepx > SIZE_PLOT_X / 4 ||
               fabs(y0) * stepy > SIZE_PLOT_Y / 4) {
            stepx /= 2;
            stepy /= 2;
        }
    }

    double y = NAN;
    for (int i = -SIZE_PLOT_X / 2; i < SIZE_PLOT_X / 2; i++) {
        y = countEq(koefs, i / stepx) * stepy;
        if (y < SIZE_PLOT_Y / 2 && y > -SIZE_PLOT_Y / 2)
            plot[(int)(SIZE_PLOT_Y / 2 - y)][i + SIZE_PLOT_X / 2] = '*';
    }

    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        printf("%.*s\n", SIZE_PLOT_X, plot[i]);
    }
}
