#ifndef _PARSER_H
#define _PARSER_H

#include <stdint.h>

#define MAX_ARGS_PER_FLAG 10 // максимальное количество аргументов у флага из командной строки

typedef struct {
    const char* name; 
    uint64_t mask;
    void (*function)(const int argc, const char *argv[], void* arg_values);
    const char* usage; 
} MyFlag;

// парсер аргументов командной строки
uint64_t parseArgs(int argc, char const *argv[], void* arg_values, int k_flags, const MyFlag* flags_list);
/// process one flag
void processFlagString(const int argc, char const *argv[], void *arg_values, int k_flags, const MyFlag* flags_list, uint64_t *flags);

#endif
