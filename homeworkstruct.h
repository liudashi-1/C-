#ifndef UNTITLED8_HOMEWORKSTRUCT_H
#define UNTITLED8_HOMEWORKSTRUCT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_USERNAME 20//最长用户名
#define MAX_PASSWORD 20//最长密码
#define MAX_CLUB_NAME 30//最大俱乐部命名
#define MAX_NAME 20//最大名字命名
#define MAX_DESCRIPTION 50
#define MAX_RECORD 1000//最大记录数据
#define XOR_KEY 0x3A// 异或加密密钥
//用户类型
typedef enum {
    user_aministrator,// 管理员
    user_president,// 社长
    user_member// 普通成员
} usertype;
// 财务流水类型
typedef enum {
    finance_income,// 收入
    finance_outcome// 支出
} financetype;
// 社团成员职位类型
typedef enum {
    member_common,// 普通成员
    member_staff,// 干事
    member_head// 部长
} memberposition;
// 1. 用户信息结构体
typedef struct {
    char username[MAX_USERNAME];// 用户名
    char password[MAX_PASSWORD];// 加密密码
    usertype type;// 用户权限类型
    char club_name[MAX_CLUB_NAME];// 所属社团
} userinformation;
// 2. 社团成员链表结构体
typedef struct clubmembernode {
    char name[MAX_NAME];// 成员姓名
    memberposition position;// 成员职位
    struct clubmembernode *next;// 下一个节点指针
} clubmembernode;
// 3. 社团信息结构体
typedef struct {
    char name[MAX_CLUB_NAME];// 社团名称
    char establish_time[20];// 成立时间
    char president_name[MAX_NAME];// 社长姓名
    clubmembernode *member_head;// 社团成员链表头节点
} clubinformation;
// 4. 财务流水记录结构体
typedef struct {
    char club_name[MAX_CLUB_NAME];// 所属社团
    financetype type;// 流水类型
    float amount;// 金额
    char create_time[20];// 交易时间
    char description[MAX_DESCRIPTION];// 备注
} financerecord;

userinformation g_users[MAX_RECORD];// 用户信息数组
int g_user_count = 0;// 当前用户总数
clubinformation g_clubs[MAX_RECORD];// 社团信息数组
int g_club_count = 0;// 当前社团总数
financerecord g_finance_records[MAX_RECORD]; // 财务流水数组
int g_finance_count = 0;// 当前财务流水总数

//------------------------------用户------------------------------
// 1. 用户查找（按用户名，返回索引，-1表示不存在）
inline int user_find(const char *username) {
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
// 2. 用户新增（返回1成功，0失败：用户名已存在/超出最大记录数）
inline int user_add(const userinformation *user) {
    if (user == NULL || g_user_count >= MAX_RECORD) {
        return 0;
    }
    // 检查用户名是否重复
    if (user_find(user->username) != -1) {
        return 0;
    }
    // 拷贝数据到全局数组
    memcpy(&g_users[g_user_count], user, sizeof(userinformation));
    g_user_count++;
    return 1;
}
// 3. 用户修改（按用户名，返回1成功，0失败：用户不存在）
inline int user_update(const char *username, const userinformation *new_user) {
    if (username == NULL || new_user == NULL) {
        return 0;
    }
    int index = user_find(username);
    if (index == -1) {
        return 0;
    }
    // 覆盖原有数据（用户名不可修改，确保一致性）
    memcpy(&g_users[index], new_user, sizeof(userinformation));
    return 1;
}
// 4. 用户删除（按用户名，返回1成功，0失败：用户不存在）
inline int user_delete(const char *username) {
    if (username == NULL || strlen(username) == 0) {
        return 0;
    }
    int index = user_find(username);
    if (index == -1) {
        return 0;
    }
    // 数组前移覆盖（删除元素）
    for (int i = index; i < g_user_count - 1; i++) {
        memcpy(&g_users[i], &g_users[i+1], sizeof(userinformation));
    }
    g_user_count--;
    return 1;
}
// 5. 用户数据保存到文件（二进制持久化，路径：users.dat）
inline void user_save_to_file() {
    FILE *f = fopen("users.dat", "wb");
    if (f == NULL) {
        printf("【错误】用户数据保存失败，无法打开文件！\n");
        return;
    }
    // 先保存用户总数，再保存用户数组
    fwrite(&g_user_count, sizeof(int), 1, f);
    fwrite(g_users, sizeof(userinformation), g_user_count, f);
    fclose(f);
    printf("【成功】用户数据已保存，共%d条记录！\n", g_user_count);
}

#endif //UNTITLED8_HOMEWORKSTRUCT_H