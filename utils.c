#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void xor_encrypt_decrypt(char* str) {
    if (str == NULL || strlen(str) == 0) {
        return;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] ^= XOR_KEY;
    }
}

void get_current_time(char* time_str, int is_long) {
    if (time_str == NULL) {
        return;
    }
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    if (is_long) {
        sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d",
            tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    }
    else {
        sprintf(time_str, "%04d-%02d-%02d",
            tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
    }
}

void clear_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int input_int(const char* prompt) {
    int value;
    printf("%s", prompt);
    scanf("%d", &value);
    clear_input();
    return value;
}

float input_float(const char* prompt) {
    float value;
    printf("%s", prompt);
    scanf("%f", &value);
    clear_input();
    return value;
}

void wait_enter(void) {
    printf("\n按回车键继续...");
    clear_input();
}

const char* usertype2str(usertype type) {
    switch (type) {
    case user_administrator: return "管理员";
    case user_president: return "社长";
    case user_member: return "普通成员";
    default: return "未知";
    }
}

const char* financetype2str(financetype type) {
    switch (type) {
    case finance_income: return "收入";
    case finance_outcome: return "支出";
    default: return "未知";
    }
}

const char* memberposition2str(memberposition position) {
    switch (position) {
    case member_common: return "普通成员";
    case member_staff: return "干事";
    case member_head: return "部长";
    default: return "未知";
    }
}