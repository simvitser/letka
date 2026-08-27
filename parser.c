#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "parser.h"


uint64_t parseArgs(int argc, char const *argv[], void* arg_values, int k_flags, const MyFlag* flags_list) {
    assert(argv != NULL);
    assert(flags_list != NULL);

    uint64_t ans = 0;
    int i = 1;
    while (strncmp("--", argv[i], 2)) i++;

    for (; i < argc; i++) {
        const char *argv_per_flag[MAX_ARGS_PER_FLAG] = {0};
        int argc_per_flag = 1;
        argv_per_flag[0] = argv[i];

        for (i++; i < argc; i++) {
            if (strncmp("--", argv[i], 2)) {
                argv_per_flag[argc_per_flag] = argv[i];
                argc_per_flag++;
            } else {
                i--;
                break;
            }
        }
        processFlagString(argc_per_flag, argv_per_flag, arg_values, k_flags, flags_list, &ans);
    }
    return ans;
}

void processFlagString(const int argc, char const *argv[], void *arg_values, int k_flags, const MyFlag* flags_list, uint64_t *flags) {
    assert(argv != NULL);
    assert(flags_list != NULL);
    assert(flags != NULL);

    for (int i = 0; i < k_flags; i++) {
        if (!strcmp(flags_list[i].name, argv[0])) {
            *flags |= flags_list[i].mask;
            if (flags_list[i].function) flags_list[i].function(argc, argv, arg_values);
            break;
        }
    }
}

void printHelp(const int k_flags, const MyFlag* flags_list) {
    printf("Конечно! Вот хороший текст для флага --help для твоей программы: Usage:\n");
    for (int i = 0; i < k_flags; i++) {
        printf("%s: %s\n", flags_list[i].name, flags_list[i].usage);
    }
}

/* только для демонстрации что вот было так
void processFlagString(const int argc, char const *argv[], void* values) {
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
}*/

