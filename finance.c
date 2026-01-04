#include "finance.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

 // 全局变量定义
financerecord g_finance_records[MAX_RECORD];
int g_finance_count = 0;

int finance_filter_by_club(const char* club_name, financerecord* out_records, int max_out) {
    if (club_name == NULL || out_records == NULL || max_out <= 0) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < g_finance_count && count < max_out; i++) {
        if (strcmp(g_finance_records[i].club_name, club_name) == 0) {
            memcpy(&out_records[count], &g_finance_records[i], sizeof(financerecord));
            count++;
        }
    }
    return count;
}

int finance_add(const financerecord* record) {
    if (record == NULL || g_finance_count >= MAX_RECORD || record->amount <= 0) {
        return 0;
    }

    memcpy(&g_finance_records[g_finance_count], record, sizeof(financerecord));
    g_finance_count++;
    return 1;
}

float finance_calc_balance(const char* club_name) {
    if (club_name == NULL || strlen(club_name) == 0) {
        return 0.0f;
    }

    float balance = 0.0f;
    for (int i = 0; i < g_finance_count; i++) {
        if (strcmp(g_finance_records[i].club_name, club_name) == 0) {
            if (g_finance_records[i].type == finance_income) {
                balance += g_finance_records[i].amount;
            }
            else {
                balance -= g_finance_records[i].amount;
            }
        }
    }
    return balance;
}

int finance_delete(int index) {
    if (index < 0 || index >= g_finance_count) {
        return 0;
    }

    for (int i = index; i < g_finance_count - 1; i++) {
        memcpy(&g_finance_records[i], &g_finance_records[i + 1], sizeof(financerecord));
    }
    g_finance_count--;
    return 1;
}

void finance_add_interactive(void) {
    // 权限检查
    if (g_current_user.type != user_president) {
        printf("【错误】只有社长可以录入财务流水！\n");
        return;
    }

    financerecord record;
    memset(&record, 0, sizeof(financerecord));

    strcpy(record.club_name, g_current_user.club_name);

    printf("===== 财务流水录入 [%s] =====\n", record.club_name);

    // 输入流水类型
    printf("请选择流水类型：\n");
    printf("0 - 收入\n");
    printf("1 - 支出\n");
    int type_choice = input_int("选择：");

    if (type_choice < 0 || type_choice > 1) {
        printf("【错误】无效的流水类型！\n");
        return;
    }
    record.type = (financetype)type_choice;

    // 输入金额
    record.amount = input_float("请输入金额：");
    if (record.amount <= 0) {
        printf("【错误】金额必须大于0！\n");
        return;
    }

    // 如果是支出，检查余额是否足够
    if (record.type == finance_outcome) {
        float balance = finance_calc_balance(record.club_name);
        if (balance < record.amount) {
            printf("【错误】余额不足！当前余额：%.2f\n", balance);
            return;
        }
    }

    // 输入备注
    printf("请输入备注：");
    scanf("%s", record.description);
    clear_input();

    // 获取当前时间
    get_current_time(record.create_time, 1);

    // 添加记录
    if (finance_add(&record)) {
        printf("【成功】财务流水添加成功！\n");
    }
    else {
        printf("【错误】财务流水添加失败！\n");
    }
}

void finance_check_balance_interactive(void) {
    char club_name[MAX_CLUB_NAME];

    if (g_current_user.type == user_administrator) {
        printf("请输入要查看余额的社团名称：");
        scanf("%s", club_name);
        clear_input();
    }
    else {
        strcpy(club_name, g_current_user.club_name);
    }

    float balance = finance_calc_balance(club_name);
    printf("【%s】当前余额：%.2f元\n", club_name, balance);
}

void finance_export_report_interactive(void) {
    char club_name[MAX_CLUB_NAME];

    if (g_current_user.type == user_administrator) {
        printf("请输入要导出报表的社团名称：");
        scanf("%s", club_name);
        clear_input();
    }
    else {
        strcpy(club_name, g_current_user.club_name);
    }

    financerecord records[MAX_RECORD];
    int count = finance_filter_by_club(club_name, records, MAX_RECORD);

    if (count == 0) {
        printf("【提示】该社团暂无财务流水记录！\n");
        return;
    }

    // 生成文件名
    char file_name[100];
    char time_str[20];
    get_current_time(time_str, 0);
    sprintf(file_name, "%s_财务报告_%s.txt", club_name, time_str);

    FILE* fp = fopen(file_name, "w");
    if (fp == NULL) {
        printf("【错误】无法创建报表文件！\n");
        return;
    }

    // 写入报表头
    fprintf(fp, "===== %s 财务流水报告 =====\n", club_name);
    fprintf(fp, "生成时间：%s\n\n", time_str);
    fprintf(fp, "序号\t时间\t\t类型\t金额\t\t备注\n");
    fprintf(fp, "--------------------------------------------------\n");

    // 写入流水记录
    float total_income = 0, total_outcome = 0;
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d\t%s\t%s\t%.2f\t\t%s\n",
            i + 1,
            records[i].create_time,
            financetype2str(records[i].type),
            records[i].amount,
            records[i].description);

        if (records[i].type == finance_income) {
            total_income += records[i].amount;
        }
        else {
            total_outcome += records[i].amount;
        }
    }

    // 写入统计信息
    fprintf(fp, "\n===== 统计信息 =====\n");
    fprintf(fp, "总收入：%.2f元\n", total_income);
    fprintf(fp, "总支出：%.2f元\n", total_outcome);
    fprintf(fp, "当前余额：%.2f元\n", total_income - total_outcome);

    fclose(fp);
    printf("【成功】财务报表已导出到文件：%s\n", file_name);
}

void finance_save_to_file(void) {
    FILE* fp = fopen("finance.dat", "wb");
    if (fp == NULL) {
        printf("【错误】财务数据保存失败，无法打开文件！\n");
        return;
    }

    fwrite(&g_finance_count, sizeof(int), 1, fp);
    fwrite(g_finance_records, sizeof(financerecord), g_finance_count, fp);
    fclose(fp);

    printf("【成功】财务数据已保存，共%d条流水记录！\n", g_finance_count);
}

void finance_load_from_file(void) {
    FILE* fp = fopen("finance.dat", "rb");
    if (fp == NULL) {
        printf("【提示】无历史财务数据，将创建新数据！\n");
        return;
    }

    fread(&g_finance_count, sizeof(int), 1, fp);
    if (g_finance_count > MAX_RECORD) {
        g_finance_count = MAX_RECORD;
    }

    if (g_finance_count > 0) {
        fread(g_finance_records, sizeof(financerecord), g_finance_count, fp);
    }
    fclose(fp);

    printf("【成功】财务数据已加载，共%d条流水记录！\n", g_finance_count);
}

void all_data_save(void) {
    user_save_to_file();
    club_save_to_file();
    finance_save_to_file();
    printf("【成功】所有数据已完成持久化保存！\n");
}

void all_data_load(void) {
    user_load_from_file();
    club_load_from_file();
    finance_load_from_file();
    printf("【成功】所有历史数据已完成加载！\n");
}