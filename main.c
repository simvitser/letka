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
#include "parser.h"

bool GLOBAL_QUIET = false;
bool GLOBAL_DEBUG_ARGS = false;
bool GLOBAL_DEBUG_PARSE = false;

static const MyFlag FLAGS[] = {
    {"--help",           FLAG_HELP,             NULL,            "show help"},
    {"--quiet",          FLAG_QUIET,            NULL,            "disable dialog outputs"},
    {"--oldenter",       FLAG_TYPEENTER,        NULL,            "enable old enter (a b c)"},
    {"--testin",         FLAG_RUNTEST,          setTestFilename, "parametr: [file.txt] unittests from file"},
    {"--debugargs",      FLAG_DEBUGARGS,        NULL,            "debug argparse"},
    {"--debugparse",     FLAG_DEBUGPARSE,       NULL,            "debug parsing eq"},
    {"--drawplot",       FLAG_DRAWPLOT,         NULL,            "draw a plot"},
    {"--drawplotoffset", FLAG_DRAWPLOTOFFSET,   NULL,            "draw a plot by offsets"},
    {"--signup",         FLAG_SIGNUP,           NULL,            "register a user"},
    {"--randtests",      FLAG_RUNRANDOMTEST,    setNumTests,     "parametr: [num_tests] generate random tests"},
    {"--seed",           FLAG_SETSEED,          setSeed,         "parametr: [seed] set seed"},
    {"--game",           FLAG_GAME,             NULL,            "run a game to enter"}
};

int main(const int argc, const char *argv[]) {
    runUnittests();

    ArgValues values = {.filename=NULL, .num_tests=0};
    uint64_t flags = parseArgs(argc, argv, &values, STATIC_LEN(FLAGS), FLAGS);
    if (flags & FLAG_HELP) {
        printHelp(STATIC_LEN(FLAGS), FLAGS);
    }

    GLOBAL_QUIET = flags & FLAG_QUIET;
    GLOBAL_DEBUG_ARGS = flags & FLAG_DEBUGARGS;
    GLOBAL_DEBUG_PARSE = flags & FLAG_DEBUGPARSE; 
    if (flags & FLAG_RUNTEST) {
        if (!runUnittestsFromFile(values.filename)) return MAINERRORS_FAILEDTESTS;
    }
    if (flags & FLAG_RUNRANDOMTEST) {
        if (!runRandomTest(values.num_tests)) return MAINERRORS_FAILEDRANDOMTEST;
    }
    if (flags & FLAG_SIGNUP) {
        signup();
        return 0;
    }
    if (flags & FLAG_GAME) {
        startGame();
    }


    QUIET {
        printf("This not AI-generated reshalka\n");
        printf(WHITE "POL" BLUE "TO" RED "RASHKA\n" STANDART);
    }

    if (!signin())
        return MAINERRORS_FAILEDSIGNIN;

    SquareKoefs koefs = {NAN, NAN, NAN};
    while (!getKoefs(&koefs, flags & FLAG_TYPEENTER)) {
        double x1 = NAN, x2 = NAN;
        KSolves solution_type = solveSquare(koefs, &x1, &x2);
        printSolves(solution_type, x1, x2);

        if (flags & FLAG_DRAWPLOT) {
            drawPlot(koefs);
        } else if (flags & FLAG_DRAWPLOTOFFSET) {
            drawPlotOffset(koefs);
        }
    }

    QUIET printf("Bye bye! ;)\n");
    return 0;
}

bool getKoefs(SquareKoefs *koefs, bool typeenter) {
    assert(koefs != NULL);

    if (typeenter) {
        return getKoefsABC(koefs);
    }

    return getKoefsParser(koefs);
}

// [[deprecated("This is old method, use getKoefsParser")]]
bool getKoefsABC(SquareKoefs *koefs) {
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

bool getKoefsParser(SquareKoefs *koefs) {
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
        QUIET printf(RED "FAILED" STANDART " solve: a: %lg b: %lg c: %lg, x1: %lg x2: %lg, x1_ref: %lg, x2_ref: %lg, solution_type: %d, solution_type_ref: %d\n",
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

void setSeed(const int argc, const char *argv[], void *arg_values) {
    assert(argv != NULL);
    assert(arg_values != NULL);

    if (argc == 2) {
        QUIET printf("setted seed: %u\n", (uint32_t)strtol(argv[1], NULL, 10));\
        srand((uint32_t)strtol(argv[1], NULL, 10));
    }
}

void setTestFilename(const int argc, const char *argv[], void *arg_values) {
    assert(argv != NULL);
    assert(arg_values != NULL);

    if (argc == 2) {
        ((ArgValues*)arg_values)->filename = argv[1];
    }
}

void setNumTests(const int argc, const char *argv[], void *arg_values) {
    assert(argv != NULL);
    assert(arg_values != NULL);

    if (argc == 2) {
        ((ArgValues*)arg_values)->num_tests = strtol(argv[1], NULL, 10);
    }
}

bool runUnittestsFromFile(const char *filename) {
    assert(filename != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf(RED "FILE DOESN'T EXIST\n" STANDART);
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
        printf(GREEN "Tests complete!\n" STANDART);
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

    QUIET printf(GREEN "RANDOM TEST PASSED\n" STANDART);
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
    plot[0][SIZE_PLOT_X / 2] = 'y';
    plot[SIZE_PLOT_Y / 2][SIZE_PLOT_X - 1] = 'x';

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
    plot[0][SIZE_PLOT_X / 2] = 'y';
    plot[SIZE_PLOT_Y / 2][SIZE_PLOT_X - 1] = 'x';

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

void printFeel(char feel[SIZE_GAME][SIZE_GAME]) {
    if (SIZE_GAME <= 10) {
        printf("    ");
        for (int i = 0; i < SIZE_GAME; i++) printf("%d", i);
        printf("\n");
    }
    for (int i = 0; i < SIZE_GAME; i++) {
        printf("%3d %.*s\n", i, SIZE_GAME, feel[i]);
    }
}

// в планах может быть внести это в маин через iterGame(x1, x2)
void startGame() {
    printf("Let`s play: enter a square eq, and abs(roots) will be coords to attack\n");

    char feel[SIZE_GAME][SIZE_GAME] = {}; 
    memset(feel, '.', sizeof feel);
    Point targets[K_TARGETS_GAME] = {};

    for (int i = 0; i < K_TARGETS_GAME; i++) {
        targets[i] = (Point){rand() % SIZE_GAME, rand() % SIZE_GAME};
        feel[targets[i].y][targets[i].x] = 'O';
    }

    printFeel(feel);
 
    int k_targets_left = K_TARGETS_GAME;
    SquareKoefs koefs = {NAN, NAN, NAN};
    while (k_targets_left && !getKoefsABC(&koefs)) {
        double x1_double = NAN, x2_double = NAN;
        KSolves solution_type = solveSquare(koefs, &x1_double, &x2_double);
        if (solution_type != TWO_SOLVES) {
            printf("you should enter a eq with 2 solutions\n");
            continue;
        }
        
        int x1 = (int)fabs(x1_double), x2 = (int)fabs(x2_double);

        printSolves(solution_type, x1, x2);

        if (x1 >= SIZE_GAME || x2 >= SIZE_GAME) continue;

        feel[x1][x2] = 'X';
        feel[x2][x1] = 'X';

        for (int i = 0; i < K_TARGETS_GAME; i++) {
            if (x1 == targets[i].x && x2 == targets[i].y) {
                printf("Striked! Targets_left: %d\n", --k_targets_left);
            }
            if (x1 == targets[i].y && x2 == targets[i].x) {
                printf("Striked! Targets_left: %d\n", --k_targets_left);
            }
        }

        printFeel(feel);
    }
}

