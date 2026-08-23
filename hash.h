#ifndef _HASH_H
#define _HASH_H
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "colors.h"
#include "common.h"

#define MAX_LEN_LOGIN 20
#define MAX_LEN_PASSWORD 30

uint64_t hash1(const char* s);
uint64_t hash2(const char* s);

void signup();
bool signin();

#endif
