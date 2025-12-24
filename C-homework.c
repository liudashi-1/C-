├── auth.c      // 登录注册、权限控制
├── auth.h      // 头文件
├── main.c      // 主函数、菜单交互
├── utils.h     // 通用工具声明（需成员A实现）
├── club.h      // 社团相关声明（需成员A实现）
├── finance.h   // 财务相关声明（需成员A实现）
└── Makefile    // 编译脚本
#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 用户类型枚举
typedef enum {
    ADMIN,    // 社联管理员
    PRESIDENT,// 社长
    MEMBER    // 普通成员
} UserType;

// 用户结构体（需与成员A定义一致）
typedef struct {
    char username[20];   // 用户名
    char password[20];   // 加密后的密码
    UserType type;       // 用户类型
    char club_name[30];  // 所属社团（社长/成员）
    int is_login;        // 是否登录
} User;

// 全局当前登录用户
extern User current_user;

/**
 * @brief 密码异或加密
 * @param password 原始密码
 * @param key 加密密钥
 * @return 加密后的密码字符串
 */
char* encrypt_password(const char* password, int key);

/**
 * @brief 用户登录
 * @return 登录成功返回1，失败返回0
 */
int user_login();

/**
 * @brief 用户注册
 * @return 注册成功返回1，失败返回0
 */
int user_register();

/**
 * @brief 检查用户权限
 * @param required_type 所需权限类型
 * @return 有权限返回1，无返回0
 */
int check_permission(UserType required_type);

/**
 * @brief 用户登出
 */
void user_logout();

/**
 * @brief 记录系统操作日志
 * @param operation 操作描述
 */
void log_operation(const char* operation);

#endif // AUTH_H

#include "auth.h"

// 全局当前登录用户初始化
User current_user = { "", "", MEMBER, "", 0 };
#define ENCRYPT_KEY 5  // 异或加密密钥
#define LOG_FILE "log.txt"  // 日志文件

/**
 * @brief 密码异或加密
 * @param password 原始密码
 * @param key 加密密钥
 * @return 加密后的密码字符串
 */
char* encrypt_password(const char* password, int key) {
    int len = strlen(password);
    char* encrypted = (char*)malloc(len + 1);
    if (!encrypted) {
        perror("malloc failed");
        return NULL;
    }
    for (int i = 0; i < len; i++) {
        encrypted[i] = password[i] ^ key;
    }
    encrypted[len] = '\0';
    return encrypted;
}

/**
 * @brief 用户登录
 * @return 登录成功返回1，失败返回0
 */
int user_login() {
    char username[20], password[20];
    char encrypted_pwd[20];
    FILE* fp = fopen("users.dat", "rb");
    if (!fp) {
        printf("用户数据文件不存在，正在创建...\n");
        fp = fopen("users.dat", "wb+");
        if (!fp) {
            perror("创建用户文件失败");
            return 0;
        }
    }

    printf("===== 用户登录 =====\n");
    printf("请输入用户名：");
    scanf("%s", username);
    printf("请输入密码：");
    scanf("%s", password);

    // 加密输入的密码
    char* input_pwd = encrypt_password(password, ENCRYPT_KEY);
    strcpy(encrypted_pwd, input_pwd);
    free(input_pwd);

    // 读取用户文件匹配
    User temp_user;
    int found = 0;
    while (fread(&temp_user, sizeof(User), 1, fp)) {
        if (strcmp(temp_user.username, username) == 0 &&
            strcmp(temp_user.password, encrypted_pwd) == 0) {
            // 登录成功
            current_user = temp_user;
            current_user.is_login = 1;
            found = 1;
            printf("登录成功！欢迎 %s\n", username);

            // 记录登录日志
            char log_msg[100];
            sprintf(log_msg, "User %s logged in at %ld", username, time(NULL));
            log_operation(log_msg);
            break;
        }
    }

    fclose(fp);
    if (!found) {
        printf("用户名或密码错误！\n");
        return 0;
    }
    return 1;
}

/**
 * @brief 用户注册
 * @return 注册成功返回1，失败返回0
 */
int user_register() {
    User new_user;
    printf("===== 用户注册 =====\n");
    printf("请输入用户名：");
    scanf("%s", new_user.username);
    printf("请输入密码：");
    scanf("%s", new_user.password);

    // 密码加密
    char* encrypted_pwd = encrypt_password(new_user.password, ENCRYPT_KEY);
    strcpy(new_user.password, encrypted_pwd);
    free(encrypted_pwd);

    // 选择用户类型
    int type_choice;
    printf("请选择用户类型：\n");
    printf("1. 社联管理员\n2. 社团社长\n3. 普通成员\n");
    printf("输入序号：");
    while (1) {
        if (scanf("%d", &type_choice) != 1 || type_choice < 1 || type_choice > 3) {
            printf("输入无效，请重新输入（1-3）：");
            // 清空输入缓冲区
            while (getchar() != '\n');
            continue;
        }
        break;
    }

    switch (type_choice) {
    case 1:
        new_user.type = ADMIN;
        strcpy(new_user.club_name, "admin");
        break;
    case 2:
        new_user.type = PRESIDENT;
        printf("请输入所属社团名称：");
        scanf("%s", new_user.club_name);
        break;
    case 3:
        new_user.type = MEMBER;
        printf("请输入所属社团名称：");
        scanf("%s", new_user.club_name);
        break;
    }
    new_user.is_login = 0;

    // 写入用户文件
    FILE* fp = fopen("users.dat", "ab");
    if (!fp) {
        perror("打开用户文件失败");
        return 0;
    }
    fwrite(&new_user, sizeof(User), 1, fp);
    fclose(fp);

    // 记录注册日志
    char log_msg[100];
    sprintf(log_msg, "User %s registered as %d type", new_user.username, new_user.type);
    log_operation(log_msg);

    printf("注册成功！\n");
    return 1;
}

/**
 * @brief 检查用户权限
 * @param required_type 所需权限类型
 * @return 有权限返回1，无返回0
 */
int check_permission(UserType required_type) {
    if (!current_user.is_login) {
        printf("请先登录！\n");
        return 0;
    }
    // 管理员拥有所有权限
    if (current_user.type == ADMIN) {
        return 1;
    }
    // 检查所需权限
    if (current_user.type == required_type) {
        return 1;
    }
    printf("权限不足！\n");
    return 0;
}

/**
 * @brief 用户登出
 */
void user_logout() {
    if (current_user.is_login) {
        // 记录登出日志
        char log_msg[100];
        sprintf(log_msg, "User %s logged out at %ld", current_user.username, time(NULL));
        log_operation(log_msg);

        // 重置当前用户
        memset(&current_user, 0, sizeof(User));
        printf("已成功登出！\n");
    }
    else {
        printf("当前未登录！\n");
    }
}

/**
 * @brief 记录系统操作日志
 * @param operation 操作描述
 */
void log_operation(const char* operation) {
    FILE* fp = fopen(LOG_FILE, "a");
    if (!fp) {
        perror("打开日志文件失败");
        return;
    }
    time_t now = time(NULL);
    fprintf(fp, "[%s] %s\n", ctime(&now), operation);
    fclose(fp);
}
#include "auth.h"
#include "club.h"   // 成员A实现：社团管理
#include "finance.h"// 成员A实现：财务管理
#include "utils.h"  // 成员A实现：通用工具

/**
 * @brief 显示主菜单
 */
void show_main_menu() {
    printf("\n===== 校园社团管理系统 =====\n");
    printf("1. 登录\n");
    printf("2. 注册\n");
    printf("3. 退出系统\n");
    printf("请选择操作（1-3）：");
}

/**
 * @brief 管理员菜单
 */
void show_admin_menu() {
    printf("\n===== 管理员菜单 =====\n");
    printf("1. 审批新社团注册\n");
    printf("2. 注销社团\n");
    printf("3. 查看所有社团信息\n");
    printf("4. 查看系统日志\n");
    printf("5. 登出\n");
    printf("请选择操作（1-5）：");
}

/**
 * @brief 社长菜单
 */
void show_president_menu() {
    printf("\n===== 社长菜单 =====\n");
    printf("1. 管理社团成员（增删改查）\n");
    printf("2. 发布活动\n");
    printf("3. 申请经费\n");
    printf("4. 查看社团财务报表\n");
    printf("5. 登出\n");
    printf("请选择操作（1-5）：");
}

/**
 * @brief 普通成员菜单
 */
void show_member_menu() {
    printf("\n===== 普通成员菜单 =====\n");
    printf("1. 查看社团信息\n");
    printf("2. 报名参加活动\n");
    printf("3. 查询个人状态\n");
    printf("4. 登出\n");
    printf("请选择操作（1-4）：");
}

/**
 * @brief 处理管理员操作
 * @param choice 操作选择
 */
void handle_admin_operation(int choice) {
    switch (choice) {
    case 1:
        if (check_permission(ADMIN)) {
            approve_club_registration();  // 成员A实现
            log_operation("Admin approved club registration");
        }
        break;
    case 2:
        if (check_permission(ADMIN)) {
            delete_club();  // 成员A实现
            log_operation("Admin deleted a club");
        }
        break;
    case 3:
        if (check_permission(ADMIN)) {
            query_all_clubs();  // 成员A实现
        }
        break;
    case 4:
        // 查看日志
        FILE * fp = fopen("log.txt", "r");
        if (!fp) {
            perror("打开日志文件失败");
            break;
        }
        char buf[200];
        printf("\n===== 系统日志 =====\n");
        while (fgets(buf, sizeof(buf), fp)) {
            printf("%s", buf);
        }
        fclose(fp);
        break;
    case 5:
        user_logout();
        break;
    default:
        printf("输入无效，请重新选择！\n");
    }
}

/**
 * @brief 处理社长操作
 * @param choice 操作选择
 */
void handle_president_operation(int choice) {
    switch (choice) {
    case 1:
        if (check_permission(PRESIDENT)) {
            manage_club_members(current_user.club_name);  // 成员A实现
            log_operation("President managed club members");
        }
        break;
    case 2:
        if (check_permission(PRESIDENT)) {
            publish_activity(current_user.club_name);  // 成员A实现
            log_operation("President published an activity");
        }
        break;
    case 3:
        if (check_permission(PRESIDENT)) {
            apply_finance(current_user.club_name);  // 成员A实现
            log_operation("President applied for finance");
        }
        break;
    case 4:
        if (check_permission(PRESIDENT)) {
            export_finance_report(current_user.club_name);  // 成员A实现
        }
        break;
    case 5:
        user_logout();
        break;
    default:
        printf("输入无效，请重新选择！\n");
    }
}

/**
 * @brief 处理普通成员操作
 * @param choice 操作选择
 */
void handle_member_operation(int choice) {
    switch (choice) {
    case 1:
        if (check_permission(MEMBER)) {
            query_club_info(current_user.club_name);  // 成员A实现
        }
        break;
    case 2:
        if (check_permission(MEMBER)) {
            // 检测活动时间冲突（成员A实现核心逻辑）
            if (check_activity_conflict(current_user.username)) {
                printf("报名失败：同一时间段已有其他活动！\n");
            }
            else {
                register_activity(current_user.username, current_user.club_name);  // 成员A实现
                log_operation("Member registered an activity");
            }
        }
        break;
    case 3:
        if (check_permission(MEMBER)) {
            query_user_status(current_user.username);  // 成员A实现
        }
        break;
    case 4:
        user_logout();
        break;
    default:
        printf("输入无效，请重新选择！\n");
    }
}

int main() {
    int choice;
    while (1) {
        // 未登录状态显示主菜单
        if (!current_user.is_login) {
            show_main_menu();
            while (1) {
                if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
                    printf("输入无效，请重新输入（1-3）：");
                    while (getchar() != '\n');
                    continue;
                }
                break;
            }

            switch (choice) {
            case 1:
                user_login();
                break;
            case 2:
                user_register();
                break;
            case 3:
                printf("感谢使用，系统退出！\n");
                // 保存所有数据（成员A实现）
                save_all_data();
                exit(0);
            default:
                printf("输入无效，请重新选择！\n");
            }
        }
        else {
            // 已登录状态，根据用户类型显示对应菜单
            switch (current_user.type) {
            case ADMIN:
                show_admin_menu();
                scanf("%d", &choice);
                handle_admin_operation(choice);
                break;
            case PRESIDENT:
                show_president_menu();
                scanf("%d", &choice);
                handle_president_operation(choice);
                break;
            case MEMBER:
                show_member_menu();
                scanf("%d", &choice);
                handle_member_operation(choice);
                break;
            }
        }
    }
    return 0;
}
CC = gcc
CFLAGS = -Wall - Wextra - g
TARGET = club_management
OBJS = main.o auth.o club.o finance.o utils.o

# 链接所有目标文件
$(TARGET) : $(OBJS)
$(CC) $(CFLAGS) - o $(TARGET) $(OBJS)

# 编译main.c
main.o: main.c auth.h club.h finance.h utils.h
$(CC) $(CFLAGS) - c main.c

# 编译auth.c
auth.o: auth.c auth.h
$(CC) $(CFLAGS) - c auth.c

# 成员A的模块（需补充）
club.o: club.c club.h utils.h
$(CC) $(CFLAGS) - c club.c

finance.o : finance.c finance.h utils.h
$(CC) $(CFLAGS) - c finance.c

utils.o : utils.c utils.h
$(CC) $(CFLAGS) - c utils.c

# 清理编译产物
clean :
rm - f $(TARGET) $(OBJS)