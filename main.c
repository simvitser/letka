#define TESTS

#define MYDEBUGARGS  if (GLOBAL_DEBUG_PARSE)
#define MYDEBUGPARSE if (GLOBAL_DEBUG_PARSE)
#define MYDEBUGREQ   if (GLOBAL_DEBUG_SOLVE_REQ)
#define QUIET        if (!GLOBAL_QUIET) 

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

bool GLOBAL_QUIET           = false;
bool GLOBAL_DEBUG_ARGS      = false;
bool GLOBAL_DEBUG_PARSE     = false;
bool GLOBAL_DEBUG_SOLVE_REQ = false;

static const MyFlag FLAGS[] = {
    {"--help",           FLAG_HELP,             NULL,            "show help"},
    {"--quiet",          FLAG_QUIET,            NULL,            "disable dialog outputs"},
    {"--oldenter",       FLAG_TYPEENTER,        NULL,            "enable old enter (a b c)"},
    {"--testin",         FLAG_RUNTEST,          setTestFilename, "parametr: [file.txt] unittests from file"},
    {"--debugargs",      FLAG_DEBUGARGS,        NULL,            "debug argparse"},
    {"--debugparse",     FLAG_DEBUGPARSE,       NULL,            "debug parsing eq"},
    {"--debugreq",       FLAG_DEBUGREQ,         NULL,            "debug solveReq"},
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

    GLOBAL_QUIET           = flags & FLAG_QUIET;
    GLOBAL_DEBUG_ARGS      = flags & FLAG_DEBUGARGS;
    GLOBAL_DEBUG_PARSE     = flags & FLAG_DEBUGPARSE;
    GLOBAL_DEBUG_SOLVE_REQ = flags & FLAG_DEBUGREQ;

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

    double koefs[MAX_POWER] = {};
    for (int i = 0; i < MAX_POWER; i++) {
        koefs[i] = NAN;
    }
    
    int max_power = 0;
    while ((max_power = getKoefs(koefs)) != -1) {
        double roots[MAX_POWER] = {};
        for (int i = 0; i < MAX_POWER; i++) {
            roots[i] = NAN;
        }

        int k_roots = solveReq(koefs, roots, max_power);
        printSolves(k_roots, roots);

        if (flags & FLAG_DRAWPLOT) {
            drawPlot(koefs, max_power, k_roots, roots);
        }
    }

    QUIET printf("Bye bye! ;)\n");
    return 0;
}

int getKoefs(double *koefs) {
    int max_power = 0;
    assert(koefs != NULL); 

    char s_input[MAX_LEN]      = {};
    char s_result[MAX_LEN * 2] = {};
    int result_index = 1;
    do {
        for (int i = 0; i < 2 * MAX_LEN; i++) s_result[i] = '\0';
        QUIET printf("enter a eq\n>>> ");
        fgets(s_input, MAX_LEN, stdin);
        if (strchr(s_input, 'q') != NULL)
            return -1;

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
    } while (result_index == 0 || ((max_power = parseKoefs(s_result, koefs)) == -1) || !isfiniteKoefs(koefs, max_power));

    printf("START SOLVING...\n");
    return max_power;
}

bool isfiniteKoefs(double *koefs, int max_power) {
    for (int i = 0; i <= max_power; i++) if (!isfinite(koefs[i])) return false;
    return true;
}

int parseKoefs(char *s_input, double *koefs) {
    assert(s_input != NULL);
    assert(koefs != NULL);

    static double koefs_sum[MAX_POWER] = {};
    for (int i = 0; i < MAX_POWER; i++) {
        koefs_sum[i] = 0;
    }
    int max_power = 0;

    int8_t sign = 1;
    static const char x_power[]      = "x^";
    static const char x_plus[]       = "x+";
    static const char plus[]         = "+";
    static const char minus_x_plus[] = "-x+";
    static const char minus_power[]  = "-x^";

    if (s_input[0] == '+') s_input++;
    do {
        MYDEBUGPARSE printf("get: %s\n\n", s_input);
        char *parse_end = NULL;
        double num_now = strtod(s_input, &parse_end);
        if (s_input == parse_end) {
            num_now = 1;
        }
        s_input = parse_end;

        if (!strncmp(s_input, x_power, STATIC_STRLEN(x_power))) {
            s_input += STATIC_STRLEN(x_power);
            int power_now = (int)strtol(s_input, &parse_end, 10);
            if (power_now >= MAX_POWER) return -1;
            if (power_now > max_power) max_power = (int8_t)power_now;
            s_input = parse_end + 1; // скип +
            koefs_sum[power_now] += num_now * sign;
        } else if (!strncmp(s_input, x_plus, STATIC_STRLEN(x_plus))) {
            if (max_power == 0) max_power = 1;
            koefs_sum[1] += num_now * sign;
            s_input += STATIC_STRLEN(x_plus);
        } else if (!strncmp(s_input, plus, STATIC_STRLEN(plus))) {
            koefs_sum[0] += num_now * sign;
            s_input += STATIC_STRLEN(plus);
        } else if (!strncmp(s_input, minus_x_plus, STATIC_STRLEN(minus_x_plus))) {
            if (max_power == 0) max_power = 1;
            koefs_sum[1] -= num_now * sign;
            s_input += STATIC_STRLEN(minus_x_plus);
        } else if (!strncmp(s_input, minus_power, STATIC_STRLEN(minus_power))) {
            s_input += STATIC_STRLEN(minus_power);
            int power_now = (int)strtol(s_input, &parse_end, 10);
            if (power_now >= MAX_POWER) return -1;
            if (power_now > max_power) max_power = power_now;
            s_input = parse_end + 1; // скип +
            koefs_sum[power_now] -= num_now * sign;
        } else if (s_input[0] != '\0') {
            return -1;
        }

        if (*s_input == '=') {
            if (sign < 0)
                return -1;
            s_input++;
            sign *= -1;
            if (*s_input == '+')
                s_input++;
        }
    } while (*s_input != '\0');
    
    for (int i = 0; i <= max_power; i++) {
        MYDEBUGPARSE printf("%lg ", koefs_sum[i]);
        koefs[i] = koefs_sum[i];
    }
    MYDEBUGPARSE putchar('\n');
    MYDEBUGPARSE printf("max_power: %d\n", max_power);
    return max_power;
}

int solveLinear(double k, double b, double *x) {
    assert(isfinite(k));
    assert(isfinite(b));
    assert(x != NULL);

    if (isZero(k)) {
        if (isZero(b))
            return INF_SOLVES;
        return 0;
    }

    *x = -b / k;
    return 1;
}

int solveReq(double *koefs, double *roots, int max_power) {
    assert(koefs != NULL);
    assert(roots != NULL);
    
    MYDEBUGREQ {
        printf("start req: %d     ", max_power);
        for (int i = 0; i <= max_power; i++) printf("%lg ", koefs[i]);
        putchar('\n');
    }

    if (max_power == 0) {
        if (isZero(koefs[0])) return INF_SOLVES;
        return 0;
    }

    if (max_power == 1) {
        return solveLinear(koefs[1], koefs[0], roots);
    }

    double koefs_dx[MAX_POWER] = {};
    for (int i = 1; i <= max_power; i++) {
        koefs_dx[i - 1] = koefs[i] * i;
    }

    double roots_dx[MAX_POWER] = {};

    int k_roots_dx = solveReq(koefs_dx, roots_dx + 1, max_power - 1);
    roots_dx[k_roots_dx + 1] = 1e9;
    roots_dx[0] = -1e9;
    k_roots_dx++;

    MYDEBUGREQ for (int i = 0; i <= k_roots_dx; i++) printf("%lg ", roots_dx[i]);
    MYDEBUGREQ putchar('\n');

    int k_roots = 0;

    for (int i = 0; i < k_roots_dx; i++) {
        double root = findRoot(roots_dx[i], roots_dx[i + 1], koefs, max_power);
        MYDEBUGREQ printf("find root: %lg,    max_power: %d,    already k_roots: %d\n", root, max_power, k_roots);

        if (isfinite(root)) {
            bool matched = false;
            for (int j = 0; j < k_roots; j++) {
                MYDEBUGREQ printf("<><><   %lg    %lg    %d\n", root, roots[j], isEqual(root, roots[j]));
                if (isEqualRoots(root, roots[j])) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                roots[k_roots] = root;
                k_roots++;
            }
        }
    } 

    return k_roots;
}

double findRoot(double l, double r, double *koefs, int max_power) {
    assert(koefs != NULL);

    MYDEBUGREQ printf("want find roots, l: %lg, r: %lg, max_power: %d\n", l, r, max_power);

    if ((countEq(koefs, max_power, l) > EPS)  && (countEq(koefs, max_power, r) > EPS))  return NAN;
    if ((countEq(koefs, max_power, l) < -EPS) && (countEq(koefs, max_power, r) < -EPS)) return NAN;

    int8_t sign = countEq(koefs, max_power, l) > countEq(koefs, max_power, r) ? -1 : 1;
    while (r - l >= EPS) {
        double m = (l + r) / 2;
        if (countEq(koefs, max_power, m) * sign > 0) {
            r = m;
        } else {
            l = m;
        }
    }
    MYDEBUGREQ printf("%lg   %lg    bin\n", l, r);
    if (fabs(countEq(koefs, max_power, l)) <= EPS_ROOT) return l;
    return NAN;
}

void printSolves(int k_roots, double* roots) {
    QUIET printf("get k_roots: %d\n", k_roots);
    if (k_roots == INF_SOLVES) {
        printf("inf solves\n");
        return;
    }
    for (int i = 0; i < k_roots; i++) {
        printf("%.4lf ", roots[i]);
    }
    putchar('\n');
}

bool runUnittest(TestCase test) {
    double roots[MAX_POWER] = {};
    bool passed = true;
    int k_roots = solveReq(test.koefs, roots, test.max_power);
    if (k_roots == test.k_roots) {
        for (int i = 0; i < k_roots; i++) {
            if (!isEqualRoots(roots[i], test.roots[i])) {
                passed = false;
                break;
            }
        }
    } else passed = false;

    if (!passed) {
        QUIET printf(RED "FAILED" STANDART);
        for (int i = 0; i < test.max_power; i++) {
            QUIET printf("%lg ", test.koefs[i]);
        }
        QUIET putchar('\n');
    }
    return passed;
}

void runUnittests() {
#ifdef TESTS
    double koefs[] = {-20.0, 0.0, 0.0, 1.0, 1.0};
    double roots[] = {-2.4168, 1.9028};
    assert(runUnittest((TestCase){
        koefs,
        .max_power = 4,  .k_roots = 2,
        roots
    }));
    double koefs2[] = {-20.0, 0.0, 0.0, 1.0, -1.0};
    assert(runUnittest((TestCase){
        koefs2,
        .max_power = 4,  .k_roots = 0,
        NULL
    }));
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

    double koefs[MAX_POWER] = {}, roots[MAX_POWER] = {};
    int k_roots = 0, max_power = 0;
    bool passed = true;
    printf("starting tests...\n");
    while (fscanf(file, "%d", &max_power) == 1) {
        for (int i = 0; i <= max_power; i++) {
            fscanf(file, "%lg", koefs + i);
        }
        
        fscanf(file, "%d", &k_roots);
        for (int i = 0; i < k_roots; i++) {
            fscanf(file, "%lg", roots + i);
        }

        bool ans = runUnittest((TestCase){
            koefs, max_power, k_roots, roots
        }); 

        passed = passed && ans;
    }

    if (passed) {
        printf(GREEN "Tests complete!\n" STANDART);
    }
    fclose(file);
    return passed;
}

bool runTest(double *koefs, int max_power) {
    double roots[MAX_POWER] = {};
    int k_roots = solveReq(koefs, roots, max_power);
    if (!checkTestAnswer(koefs, max_power, k_roots, roots)) {
        QUIET printf(RED "FAILED " STANDART "solve: ");
        for (int i = 0; i <= max_power; i++) {
            QUIET printf("%lg ", koefs[i]);
        }
        QUIET putchar('\n');
        return false;
    }
    return true;
}

bool runRandomTest(long num_tests) {
    QUIET printf("starting random tests...\n");
    double koefs[MAX_POWER] = {};

    for (int max_power = 1; max_power < MAX_POWER; max_power++) {
        QUIET printf("Started rnd max_power: %d\n", max_power);
        for (int index_test = 0; index_test < num_tests; index_test++) {
            for (int i = 0; i <= max_power; i++) {
                koefs[i] = randDouble();
            }
            if (!runTest(koefs, max_power)) return false;
        }
    }

    QUIET printf(GREEN "RANDOM TEST PASSED\n" STANDART);
    return true;
}

double countEq(double *koefs, int max_power, double x) {
    double ans = 0;
    double x_ = 1;
    for (int i = 0; i <= max_power; i++) {
        ans += koefs[i] * x_;
        x_ *= x;
    }
    return ans;
}

bool checkTestAnswer(double *koefs, int max_power, int k_roots, double* roots) {
    for (int i = 0; i < k_roots; i++) {
        double ans = countEq(koefs, max_power, roots[i]);
        if (!isZeroAns(ans)) {
            return false;
        }
    }
    return true;
}

void drawPlot(double *koefs, int max_power, int k_roots, double *roots) {
    assert(koefs != NULL);
    assert(isfiniteKoefs(koefs, max_power));
    assert(roots != NULL);
    assert(isfiniteKoefs(roots, k_roots - 1));

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

    double stepx = 20, stepy = 1.6;
    
    double y = NAN;
    for (int i = -SIZE_PLOT_X / 2; i < SIZE_PLOT_X / 2; i++) {
        y = countEq(koefs, max_power, i / stepx) * stepy;
        if (y < SIZE_PLOT_Y / 2 && y > -SIZE_PLOT_Y / 2)
            plot[(int)(SIZE_PLOT_Y / 2 - y)][i + SIZE_PLOT_X / 2] = '*';
    }

    for (int i = 0; i < k_roots; i++) plot[SIZE_PLOT_Y / 2][(int)(roots[i] * stepx) + SIZE_PLOT_X / 2] = '0';

    for (int i = 0; i < SIZE_PLOT_Y; i++) {
        printf("%.*s\n", SIZE_PLOT_X, plot[i]);
    }
}

void printFeel(char feel[SIZE_GAME][SIZE_GAME]) {
    assert(feel != NULL);

    if (SIZE_GAME <= 10) { // не больше 10 чисел - пишем верхнюю строку
        printf("    ");
        for (int i = 0; i < SIZE_GAME; i++) printf("%d", i);
        printf("\n");
    }

    for (int i = 0; i < SIZE_GAME; i++) {
        printf("%3d %.*s\n", i, SIZE_GAME, feel[i]);
    }
}

void startGame() {
    printf("Let`s play: enter a eq with 2 solutions, and abs(roots) will be coords to attack\n");

    char feel[SIZE_GAME][SIZE_GAME] = {}; 
    memset(feel, '.', sizeof feel);
    Point targets[K_TARGETS_GAME] = {};

    for (int i = 0; i < K_TARGETS_GAME; i++) {
        targets[i] = (Point){rand() % SIZE_GAME, rand() % SIZE_GAME};
        feel[targets[i].y][targets[i].x] = 'O';
    }

    printFeel(feel);
 
    int k_targets_left = K_TARGETS_GAME;
    double koefs[MAX_POWER] = {};
    double roots[MAX_POWER] = {};
    int max_power = 0;
    while (k_targets_left && (max_power = getKoefs(koefs)) != -1) { 
        int k_roots = solveReq(koefs, roots, max_power);
        if (k_roots != 2) {
            printf("you should enter a eq with 2 solutions\n");
            continue;
        }
        
        printSolves(k_roots, roots);
        int x1 = (int)fabs(roots[0]), x2 = (int)fabs(roots[1]);

        if (x1 >= SIZE_GAME || x2 >= SIZE_GAME) continue;

        feel[x1][x2] = 'X';
        feel[x2][x1] = 'X';

        for (int i = 0; i < K_TARGETS_GAME; i++) {
            if (x1 == targets[i].x && x2 == targets[i].y) {
                targets[i] = (Point){SIZE_GAME, SIZE_GAME};
                printf("Striked! Targets_left: %d\n", --k_targets_left);
            }
            if (x1 == targets[i].y && x2 == targets[i].x) {
                targets[i] = (Point){SIZE_GAME, SIZE_GAME};
                printf("Striked! Targets_left: %d\n", --k_targets_left);
            }
        }

        printFeel(feel);
    }
    if (k_targets_left == 0) {
        printf(GREEN "YOU WIN\n" STANDART);
    }
}

bool isEqualRoots(double a, double b) {return isZeroAns(a - b);}
bool isZeroAns(double a) {return fabs(a) <= EPS_ROOT;}
