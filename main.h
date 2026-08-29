/** 
 * \file  
 * \brief основная программа
 *
*/
#ifndef _MAIN_H
#define _MAIN_H

#include <stdbool.h>
#include <stdint.h>

// #define quietColoredPrintf(color, smth) if (!GLOBAL_QUIET) coloredPrintf(color, smth)
// #define quietPrintf(smth) if (!GLOBAL_QUIET) printf(smth)    // another version
#define myassert(esp)                                                          \
    if (!(esp)) {                                                              \
        fprintf(stderr, "ASSSSSERT: %s! line: %d, file: %s, func: %s\n", #esp, \
                __LINE__, __FILE__, __func__);                                 \
        abort();                                                               \
    }
 
#define MAX_LEN 100 // максимальная длина вводимого уравнения
#define MAX_POWER 35 // максимальное количество слагаемых многочлена для ввода

#define SIZE_PLOT_Y 41 // размер графика
#define SIZE_PLOT_X 121


#define SIZE_GAME 6
#define K_TARGETS_GAME 3

#define INF_SOLVES -1
#define EPS_ROOT 1e-4 // точность для сравнения решений

/**
 * \brief функция сравнения корней
 * \param[in] a первый корень
 * \param[in] b второй корень
 * \return равны ли корни
 */
bool isEqualRoots(double a, double b);

/**
 * \brief Проверки равенства нулю с меньшей точностью
 * \param[in] a число
 * \return a == 0
 */
bool isZeroAns(double a);

/**
 * \brief структура для тестов
 */
typedef struct {
    double *koefs;
    int max_power;
    int k_roots;
    double *roots;
} TestCase;

/**
 * \brief структура для возвращаемых значений
 */
typedef struct {
    const char* filename;
    long num_tests;
} ArgValues; 

/**
 * \brief структура точки для игры
 */
typedef struct {
    int y, x;
} Point;

/**
 * \brief битовые маски флагов парсера аргументов
 */
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
    FLAG_HELP = (1U << 10),
    FLAG_GAME = (1U << 11),
    FLAG_DEBUGREQ = (1U << 12)
};

/**
 * \brief ошибки фунции main
 */
enum MAINERRORS {
    MAINERRORS_FAILEDTESTS = 67,
    MAINERRORS_FAILEDRANDOMTEST,
    MAINERRORS_FAILEDSIGNIN
};

/** 
 * \brief Функция запуска юнит тестов
 */
void runUnittests();

/**
 * \brief запуск одного юнит теста
 * \param[in] test данные теста
 * \return пройден ли тест
 */
bool runUnittest(TestCase test);

/** 
 * \brief запуск теста с проверкой постановкой
 * \param[in] koefs коэффициенты уравнения
 * \param[in] max_power степень уравнения
 * \return пройден ли тест
 */
bool runTest(double *koefs, int max_power);


/* 
 * \brief юнит тесты из файла
 * \param[in] filename имя файла
 * \return пройдены ли тесты
 */
bool runUnittestsFromFile(const char *filename);

/** 
 * \brief ввод уравнения с парсером
 * \param[out] koefs коэффициенты уравнения
 * \return степень многочлена
 */
int getKoefs(double koefs[]);

/** 
 * \brief парсер коэффициентов из преобразованной строки
 * \param[in] s строка для обработки
 * \param[out] koefs коэффициенты уравнения
 * \return степень многочлена
 */
int parseKoefs(char *s, double koefs[]);

/**
 * \brief проверка всех коэффициентов на isfinite
 * \param[in] koefs коэффициенты уравнения
 * \param[in] max_power степень уравнения
 * \return результат isfinite от всех корней
 */
bool isfiniteKoefs(double *koefs, int max_power);

/** 
 * \brief решатель линейных уравнений
 * \param[in] k, b коэффициенты уравнения
 * \param[out] x решение
 * \return количество решений
 */
int solveLinear(double k, double b, double *x);

/**
 * \brief решатель квадратного уравнения
 * \param[in] koefs коэффициенты уравнения
 * \param[out] roots решения уравнения
 * \param[in] max_power степень уравнения
 */
int solveReq(double *koefs, double *roots, int max_power);

/**
 * \brief бинарный поиск корня
 * \param[in] l, r границы поиска
 * \param[in] koefs коэффициенты уравнения
 * \param[in] max_power степень уравнения
 * \return корень
 */
double findRoot(double l, double r, double *koefs, int max_power);

/**
 * \brief вывод корней
 * \param[in] k_roots количество решений
 * \param[in] roots решения уравнения
 */
void printSolves(int k_roots, double* roots);

/**
 * \brief расчет многочлена y = ... + ax^2 + bx + x
 * \param[in] koefs коэффициенты уравнения
 * \param[in] max_power степень уравнения
 * \param[in] x x
 * \return y(x)
 */
double countEq(double *koefs, int max_power, double x);


/** 
 * \brief запуск рандомных тестов
 * \param[in] num_tests количество тестов
 * \return пройдены ли тесты
 */
bool runRandomTest(long num_tests);

/**
 * \brief построение графика функции
 * \param[in] koefs коэффициенты уравнения
 * \param[in] max_power степень уравнения
 * \param[in] k_roots количество решений
 * \param[in] roots решения уравнения
 */
void drawPlot(double *koefs, int max_power, int k_roots, double* roots);

/** 
* \brief проверка правильности ответа подстановкой
* \param[in] koefs коэффициенты уравнения
* \param[in] max_power степень уравнения
* \param[in] k_roots количество решений
* \param[in] roots решения уравнения
* \return правильный ли ответ
*/
bool checkTestAnswer(double *koefs, int max_power, int k_roots, double* roots);

// функции для флагов
void setSeed(const int argc, const char *argv[], void *arg_values);
void setTestFilename(const int argc, const char *argv[], void *arg_values);
void setNumTests(const int argc, const char *argv[], void *arg_values);

/**
* \brief функция вывода поля на экран
* \param[in] feel поле
*/
void printFeel(char feel[SIZE_GAME][SIZE_GAME]);

/**
* \brief функция запуска игры
*/
void startGame();

#endif
