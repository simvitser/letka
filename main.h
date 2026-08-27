#ifndef _MAIN_H
#define _MAIN_H

#include <stdbool.h>
#include <stdint.h>

// #define quietColoredPrintf(color, smth) if (!GLOBAL_QUIET) coloredPrintf(color, smth)
// #define quietPrintf(smth) if (!GLOBAL_QUIET) printf(smth)    // another version
#define QUIET if (!GLOBAL_QUIET) 
#define myassert(esp)                                                          \
    if (!(esp)) {                                                              \
        fprintf(stderr, "ASSSSSERT: %s! line: %d, file: %s, func: %s\n", #esp, \
                __LINE__, __FILE__, __func__);                                 \
        abort();                                                               \
    }
 
#define MAX_LEN 100 // максимальная длина вводимого уравнения
#define SIZE_PLOT_Y 41 // размер графика
#define SIZE_PLOT_X 121

typedef enum {
    ZERO_SOLVES,
    ONE_SOLVE,
    TWO_SOLVES,
    INF_SOLVES
} KSolves;

typedef struct {
    double a, b, c;
} SquareKoefs;

typedef struct {
    SquareKoefs koefs;
    KSolves solution_type;
    double x1, x2;
} TestCase;

typedef struct {
    const char* filename;
    long num_tests;
    uint32_t seed;
    uint64_t flags;
} ArgValues; // структура для возвращаемых значений

enum FLAG_BYTES {
    FLAG_QUIET = (1U << 0), 
    FLAG_TYPEENTER = (1U << 1), 
    FLAG_RUNTEST = (1U << 2), 
    FLAG_DEBUGARGS = (1U << 3), 
    FLAG_DEBUGPARSE = (1U << 4), 
    FLAG_SIGNUP = (1U << 5), 
    FLAG_RUNRANDOMTEST = (1U << 6),
    FLAG_DRAWPLOT = (1U << 7),
    FLAG_DRAWPLOTOFFSET = (1U << 8),
    FLAG_SETSEED = (1U << 9),
    FLAG_HELP = (1U << 10)
};

enum MAINERRORS {
    MAINERRORS_FAILEDTESTS = 67,
    MAINERRORS_FAILEDRANDOMTEST,
    MAINERRORS_FAILEDSIGNIN
};

// Функция запуска юнит тестов
void runUnittests();
// запуск одного юнит теста
bool runUnittest(TestCase test);
// запуск теста с проверкой постановкой
bool runTest(SquareKoefs koefs);
// юнит тесты из файла
bool runUnittestsFromFile(const char *filename);

/** 
 * @brief старое, пытался doxygen но не создает
 * функция для ввода 3 чисел с плавающей точкой
 *
 */
bool getKoefsABC(SquareKoefs* koefs);
// ввод уравнения с парсером
bool getKoefsParser(SquareKoefs *koefs);
// парсер коэффициентов из преобразованной строки
bool parseKoefs(char *s, SquareKoefs *koefs);
// ввод коэффициентов, запускает getKoefsABC или getKoefsParser по флагу 
bool getKoefs(SquareKoefs *koefs, bool typeenter);

// решатель квадратного уравнения
KSolves solveLinear(double k, double b, double *x);
KSolves solveSquare(SquareKoefs koefs, double *x1, double *x2);

// вывод корней
void printSolves(KSolves solution_type, double x1, double x2);

// расчет ax^2 + bx + c
double countEq(SquareKoefs koefs, double x);

// запуск рандомных тестов
bool runRandomTest(long num_tests);

// построение графика на основе scale
void drawPlot(SquareKoefs koefs);
// постороение графика на основе смещений
void drawPlotOffset(SquareKoefs koefs);

// получение правильного типа решения на основе вершины параболы
KSolves getRightSolutionType(SquareKoefs koefs);
// проверка правильности ответа подстановкой
bool checkTestAnswer(SquareKoefs koefs, KSolves solution_type_ans, double x1, double x2);

// функции для флагов
void setSeed(const int argc, const char *argv[], void *arg_values);
void setTestFilename(const int argc, const char *argv[], void *arg_values);
void setNumTests(const int argc, const char *argv[], void *arg_values);

#endif
