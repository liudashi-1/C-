#ifndef UNTITLED8_HOMEWORKFUN_H
#define UNTITLED8_HOMEWORKFUN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void get_current_time(char *time_str,int is) {
    if (time_str == NULL) {
        return;

    }
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    if (is) {
        sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d",
                tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
                tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    } else {
        sprintf(time_str, "%04d-%02d-%02d",
                tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
    }
}
#endif //UNTITLED8_HOMEWORKFUN_H