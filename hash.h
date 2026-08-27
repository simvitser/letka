#ifndef _HASH_H
#define _HASH_H
#include <stdint.h>
#include <stdbool.h>

#define MAX_LEN_LOGIN 20
#define MAX_LEN_PASSWORD 30

// хэш функция на основе переполнения uint64_t
uint64_t hash1(const char* s);

// хэш функция на основе остатка деления на 1e9 - 3
uint64_t hash2(const char* s);

void signup();
bool signin();

#endif
