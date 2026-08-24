#include "hash.h"


uint64_t hash1(const char *s) {
    assert(s != NULL);
    uint64_t p = 1;
    uint64_t ans = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        ans += p * (uint64_t)s[i];
        p *= 67;
    }
    return ans;
}

uint64_t hash2(const char *s) {
    assert(s != NULL);
    uint64_t p = 1;
    uint64_t ans = 0;
    uint32_t q = 1e9 - 3;
    for (int i = 0; s[i] != '\0'; i++) {
        ans = (ans + (p * (uint64_t)s[i]) % q) % q;
        p = (p * 127) % q;
    }
    return ans;
}

void signup() {
    char login[MAX_LEN_LOGIN] = {0};
    char password1[MAX_LEN_PASSWORD] = {0};
    char password2[MAX_LEN_PASSWORD] = {0};
    printf("Enter a login\n>>> ");
    scanf("%s", login);
    printf("Enter a password\n>>> ");
    scanf("%s", password1);
    printf("Repeat password\n>>> ");
    scanf("%s", password2);
    if (strcmp(password1, password2)) {
        coloredPrintf(RED, "THEY MUST BE SAME!\n");
        return;
    }
    uint64_t h1 = hash1(password1);
    uint64_t h2 = hash2(password1);

    char login_temp[MAX_LEN_LOGIN] = {0};
    FILE *file = fopen("baza.txt", "a+");
    if (file == NULL) {
        perror("Something went wrong, please, try later");
        return;
    }
    uint64_t h1_temp = 0;
    uint64_t h2_temp = 0;
    while (fscanf(file, "%s %lu %lu", login_temp, &h1_temp, &h2_temp) == 3) { 
        if (!strcmp(login_temp, login)) {
            coloredPrintf(RED, "Already in database\n");
            return;
        }
        if (h1 == h1_temp && h2 == h2_temp) {
            printf("This password already use a %s\n", login_temp);
            return;
        }
    }
    fprintf(file, "%s %lu %lu\n", login, h1, h2);
    printf("registred!\n");
    fclose(file);
}

bool signin() {
    char login[MAX_LEN_LOGIN] = {0};
    char password[MAX_LEN_PASSWORD] = {0};
    char login_temp[MAX_LEN_LOGIN] = {0};
    printf("Please, enter login\n>>> ");
    scanf("%s", login);
    clearInputBuffer();
    printf("Password\n>>> ");
    scanf("%s", password);
    clearInputBuffer();
    uint64_t h1 = hash1(password);
    uint64_t h2 = hash2(password);
    uint64_t h1_temp = 0;
    uint64_t h2_temp = 0;
    FILE *file = fopen("baza.txt", "r");
    if (file == NULL) {
        perror("Something went wrong, please, try later");
        return false;
    }
    while (fscanf(file, "%s %lu %lu", login_temp, &h1_temp, &h2_temp) == 3) { 
        if (!strcmp(login_temp, login)) {
            if (h1 == h1_temp && h2 == h2_temp) {
                coloredPrintf(GREEN, "CORRECT\n");
                return true;
            } else {
                coloredPrintf(RED, "UNCORRECT PASSWORD\n");
                return false;
            }
        }
    }
    coloredPrintf(RED, "Who are you? sing up!\n");
    fclose(file);
    return false;
}
