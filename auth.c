#include "auth.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

 // 全局变量定义
userinformation g_users[MAX_RECORD];
int g_user_count = 0;
userinformation g_current_user;

int user_login(const char* username, const char* password) {
    if (username == NULL || password == NULL) {
        return 0;
    }

    int index = user_find(username);
    if (index == -1) {
        return 0;
    }

    // 验证密码
    char encrypted_password[MAX_PASSWORD];
    strcpy(encrypted_password, password);
    xor_encrypt_decrypt(encrypted_password);

    if (strcmp(g_users[index].password, encrypted_password) == 0) {
        memcpy(&g_current_user, &g_users[index], sizeof(userinformation));
        return 1;
    }

    return 0;
}

void user_register(void) {
    if (g_user_count >= MAX_RECORD) {
        printf("【错误】用户数量已达上限！\n");
        return;
    }

    userinformation new_user;
    memset(&new_user, 0, sizeof(userinformation));

    printf("===== 用户注册 =====\n");

    // 输入用户名
    printf("请输入用户名（长度<%d）：", MAX_USERNAME);
    scanf("%s", new_user.username);
    clear_input();

    // 检查用户名是否已存在
    if (user_find(new_user.username) != -1) {
        printf("【错误】用户名已存在！\n");
        return;
    }

    // 输入密码
    char password[MAX_PASSWORD];
    printf("请输入密码（长度<%d）：", MAX_PASSWORD);
    scanf("%s", password);
    clear_input();

    // 加密密码
    strcpy(new_user.password, password);
    xor_encrypt_decrypt(new_user.password);

    // 选择用户类型
    printf("请选择用户类型：\n");
    printf("0 - 管理员\n");
    printf("1 - 社长\n");
    printf("2 - 普通成员\n");
    printf("选择：");
    int type_choice;
    scanf("%d", &type_choice);
    clear_input();

    if (type_choice < 0 || type_choice > 2) {
        printf("【错误】无效的用户类型！\n");
        return;
    }
    new_user.type = (usertype)type_choice;

    // 如果是社长或成员，需要指定所属社团
    if (new_user.type != user_administrator) {
        printf("请输入所属社团名称：");
        scanf("%s", new_user.club_name);
        clear_input();
    }

    // 添加用户
    if (user_add(&new_user)) {
        printf("【成功】用户注册成功！\n");
    }
    else {
        printf("【错误】用户注册失败！\n");
    }
}

int user_find(const char* username) {
    if (username == NULL || strlen(username) == 0) {
        return -1;
    }
    for (int i = 0; i < g_user_count; i++) {
        if (strcmp(g_users[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

int user_add(const userinformation* user) {
    if (user == NULL || g_user_count >= MAX_RECORD) {
        return 0;
    }

    if (user_find(user->username) != -1) {
        return 0;
    }

    memcpy(&g_users[g_user_count], user, sizeof(userinformation));
    g_user_count++;
    return 1;
}

int user_update(const char* username, const userinformation* new_user) {
    if (username == NULL || new_user == NULL) {
        return 0;
    }

    int index = user_find(username);
    if (index == -1) {
        return 0;
    }

    memcpy(&g_users[index], new_user, sizeof(userinformation));
    return 1;
}

int user_delete(const char* username) {
    if (username == NULL || strlen(username) == 0) {
        return 0;
    }

    int index = user_find(username);
    if (index == -1) {
        return 0;
    }

    for (int i = index; i < g_user_count - 1; i++) {
        memcpy(&g_users[i], &g_users[i + 1], sizeof(userinformation));
    }
    g_user_count--;
    return 1;
}

void user_save_to_file(void) {
    FILE* f = fopen("users.dat", "wb");
    if (f == NULL) {
        printf("【错误】用户数据保存失败，无法打开文件！\n");
        return;
    }

    fwrite(&g_user_count, sizeof(int), 1, f);
    fwrite(g_users, sizeof(userinformation), g_user_count, f);
    fclose(f);
    printf("【成功】用户数据已保存，共%d条记录！\n", g_user_count);
}

void user_load_from_file(void) {
    FILE* fp = fopen("users.dat", "rb");
    if (fp == NULL) {
        printf("【提示】无历史用户数据，将创建新数据！\n");
        return;
    }

    fread(&g_user_count, sizeof(int), 1, fp);
    if (g_user_count > MAX_RECORD) {
        g_user_count = MAX_RECORD;
    }

    if (g_user_count > 0) {
        fread(g_users, sizeof(userinformation), g_user_count, fp);
    }
    fclose(fp);
    printf("【成功】用户数据已加载，共%d条记录！\n", g_user_count);
}