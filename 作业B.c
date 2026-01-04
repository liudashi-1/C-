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
    struct clubmembernode* next;// 下一个节点指针
} clubmembernode;
// 3. 社团信息结构体
typedef struct {
    char name[MAX_CLUB_NAME];// 社团名称
    char establish_time[20];// 成立时间
    char president_name[MAX_NAME];// 社长姓名
    clubmembernode* member_head;// 社团成员链表头节点
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

// -------------------- 工具函数封装 --------------------
// 异或加密/解密函数（对称加密，加密解密同一逻辑）
void xor_encrypt_decrypt(char* str) {
    if (str == NULL || strlen(str) == 0) {
        return;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] ^= XOR_KEY;
    }
}
// 获取当前时间字符串
// is_long: 1=长格式（财务流水），0=短格式（社团成立时间）
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
//------------------------------用户------------------------------
// 1. 用户查找（按用户名，返回索引，-1表示不存在）
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
// 2. 用户新增（返回1成功，0失败：用户名已存在/超出最大记录数）
int user_add(const userinformation* user) {
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
int user_update(const char* username, const userinformation* new_user) {
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
int user_delete(const char* username) {
    if (username == NULL || strlen(username) == 0) {
        return 0;
    }
    int index = user_find(username);
    if (index == -1) {
        return 0;
    }
    // 数组前移覆盖（删除元素）
    for (int i = index; i < g_user_count - 1; i++) {
        memcpy(&g_users[i], &g_users[i + 1], sizeof(userinformation));
    }
    g_user_count--;
    return 1;
}
// 5. 用户数据保存到文件
void user_save_to_file() {
    FILE* f = fopen("users.dat", "wb");
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
// 6. 用户数据从文件加载（二进制读取，路径：users.dat）
void user_load_from_file() {
    FILE* fp = fopen("users.dat", "rb");
    if (fp == NULL) {
        printf("【提示】无历史用户数据，将创建新数据！\n");
        return;
    }
    // 先加载用户总数，再加载用户数组
    fread(&g_user_count, sizeof(int), 1, fp);
    // 防止文件损坏导致的超出最大记录数
    if (g_user_count > MAX_RECORD) {
        g_user_count = MAX_RECORD;
    }
    fread(g_users, sizeof(userinformation), g_user_count, fp);
    fclose(fp);
    printf("【成功】用户数据已加载，共%d条记录！\n", g_user_count);
}
//-----------------------社团-----------------------
// 1. 社团查找（按社团名称，返回索引，-1表示不存在）
int club_find_by_name(const char* club_name) {
    if (club_name == NULL || strlen(club_name) == 0) {
        return -1;
    }
    for (int i = 0; i < g_club_count; i++) {
        if (strcmp(g_clubs[i].name, club_name) == 0) {
            return i;
        }
    }
    return -1;
}
// 2. 社团新增（返回1成功，0失败：名称已存在/超出最大记录数）
int club_add(const clubinformation* club) {
    if (club == NULL || g_club_count >= MAX_RECORD) {
        return 0;
    }
    // 检查社团名称是否重复
    if (club_find_by_name(club->name) != -1) {
        return 0;
    }
    // 拷贝数据到全局数组，初始化成员链表头节点为NULL
    memcpy(&g_clubs[g_club_count], club, sizeof(clubinformation));
    g_clubs[g_club_count].member_head = NULL;
    g_club_count++;
    return 1;
}
// 3. 社团修改（按社团名称，返回1成功，0失败：社团不存在）
int club_update(const char* club_name, const clubinformation* new_club) {
    if (club_name == NULL || new_club == NULL) {
        return 0;
    }
    int index = club_find_by_name(club_name);
    if (index == -1) {
        return 0;
    }
    // 覆盖数据（保留原有成员链表，仅更新社团基本信息）
    g_clubs[index].member_head = g_clubs[index].member_head; // 保留成员链表
    memcpy(&g_clubs[index].name, new_club->name, sizeof(g_clubs[index].name));
    memcpy(&g_clubs[index].establish_time, new_club->establish_time, sizeof(g_clubs[index].establish_time));
    memcpy(&g_clubs[index].president_name, new_club->president_name, sizeof(g_clubs[index].president_name));
    return 1;
}

void club_member_list_free(clubmembernode** clubmembernode);

// 4. 社团删除（按社团名称，返回1成功，0失败：社团不存在）
int club_delete(const char* club_name) {
    if (club_name == NULL || strlen(club_name) == 0) {
        return 0;
    }
    int index = club_find_by_name(club_name);
    if (index == -1) {
        return 0;
    }
    // 先释放该社团的成员链表内存
    club_member_list_free(&g_clubs[index].member_head);
    // 数组前移覆盖（删除社团）
    for (int i = index; i < g_club_count - 1; i++) {
        memcpy(&g_clubs[i], &g_clubs[i + 1], sizeof(clubinformation));
    }
    g_club_count--;
    return 1;
}
// -------------社团成员-------------
// 1. 成员链表：查找成员（按姓名，返回节点指针，NULL表示不存在）
clubmembernode* club_member_find(clubmembernode* head, const char* name) {
    if (head == NULL || name == NULL || strlen(name) == 0) {
        return NULL;
    }
    clubmembernode* p = head;
    while (p != NULL) {
        if (strcmp(p->name, name) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}
// 2. 成员链表：新增成员（头插法，返回1成功，0失败：成员已存在）
int club_member_add(clubmembernode** head, const char* name, memberposition position) {
    if (head == NULL || name == NULL || strlen(name) == 0) {
        return 0;
    }
    // 检查成员是否已存在
    if (club_member_find(*head, name) != NULL) {
        return 0;
    }
    // 分配新节点内存
    clubmembernode* new_node = (clubmembernode*)malloc(sizeof(clubmembernode));
    if (new_node == NULL) {
        return 0;
    }
    // 初始化新节点
    strcpy(new_node->name, name);
    new_node->position = position;
    new_node->next = *head;// 头插法，新节点指向原头节点
    *head = new_node;// 更新头节点为新节点
    return 1;
}
// 3. 成员链表：修改成员职位（按姓名，返回1成功，0失败：成员不存在）
int club_member_update_position(clubmembernode* head, const char* name, memberposition new_position) {
    if (head == NULL || name == NULL || strlen(name) == 0) {
        return 0;
    }
    clubmembernode* node = club_member_find(head, name);
    if (node == NULL) {
        return 0;
    }
    node->position = new_position;
    return 1;
}
// 4. 成员链表：删除成员（按姓名，返回1成功，0失败：成员不存在）
int club_member_delete(clubmembernode** head, const char* name) {
    if (head == NULL || *head == NULL || name == NULL || strlen(name) == 0) {
        return 0;
    }
    clubmembernode* p = *head;
    clubmembernode* pre = NULL; // 前驱节点
    // 查找目标节点及其前驱节点
    while (p != NULL && strcmp(p->name, name) != 0) {
        pre = p;
        p = p->next;
    }
    if (p == NULL) { // 未找到成员
        return 0;
    }
    // 删除节点：分头部节点和中间/尾部节点
    if (pre == NULL) { // 目标是头节点
        *head = p->next;
    }
    else { // 目标是中间或尾部节点
        pre->next = p->next;
    }
    free(p); // 释放内存
    return 1;
}
// 5. 成员链表：释放整个链表内存
void club_member_list_free(clubmembernode** head) {
    if (head == NULL || *head == NULL) {
        return;
    }
    clubmembernode* p = *head;
    while (p != NULL) {
        clubmembernode* temp = p;
        p = p->next;
        free(temp);
    }
    *head = NULL; // 头节点置空
}
// ------------- 社团模块文件IO -------------
// 1. 社团数据保存到文件
void club_save_to_file() {
    // 保存社团基本信息
    FILE* fp = fopen("clubs.dat", "wb");
    if (fp == NULL) {
        printf("【错误】社团基本数据保存失败，无法打开文件！\n");
        return;
    }
    fwrite(&g_club_count, sizeof(int), 1, fp);
    fwrite(g_clubs, sizeof(clubinformation), g_club_count, fp);
    fclose(fp);
    // 保存每个社团的成员链表
    for (int i = 0; i < g_club_count; i++) {
        char file_name[64];
        sprintf(file_name, "club_%s_members.dat", g_clubs[i].name);
        FILE* mem_fp = fopen(file_name, "wb");
        if (mem_fp == NULL) {
            printf("【错误】社团[%s]成员数据保存失败！\n", g_clubs[i].name);
            continue;
        }
        // 遍历链表，写入所有成员节点（以空节点作为结束标记）
        clubmembernode* p = g_clubs[i].member_head;
        while (p != NULL) {
            fwrite(p, sizeof(clubmembernode), 1, mem_fp);
            p = p->next;
        }
        // 写入结束标记（空节点）
        clubmembernode end_node = { 0 };
        fwrite(&end_node, sizeof(clubmembernode), 1, mem_fp);
        fclose(mem_fp);
    }
    printf("【成功】社团数据已保存，共%d个社团！\n", g_club_count);
}
// 2. 社团数据从文件加载（恢复社团基本信息和成员链表）
void club_load_from_file() {
    // 加载社团基本信息
    FILE* fp = fopen("clubs.dat", "rb");
    if (fp == NULL) {
        printf("【提示】无历史社团数据，将创建新数据！\n");
        return;
    }
    fread(&g_club_count, sizeof(int), 1, fp);
    if (g_club_count > MAX_RECORD) {
        g_club_count = MAX_RECORD;
    }
    fread(g_clubs, sizeof(clubinformation), g_club_count, fp);
    fclose(fp);
    // 加载每个社团的成员链表
    for (int i = 0; i < g_club_count; i++) {
        // 先释放原有无效链表（确保安全）
        club_member_list_free(&g_clubs[i].member_head);
        char file_name[64];
        sprintf(file_name, "club_%s_members.dat", g_clubs[i].name);
        FILE* mem_fp = fopen(file_name, "rb");
        if (mem_fp == NULL) {
            printf("【提示】社团[%s]无历史成员数据！\n", g_clubs[i].name);
            g_clubs[i].member_head = NULL;
            continue;
        }
        // 读取成员节点，直到遇到结束标记（空节点）
        clubmembernode* tail = NULL; // 尾指针（尾插法恢复链表，保持原有顺序）
        clubmembernode temp_node;
        while (1) {
            fread(&temp_node, sizeof(clubmembernode), 1, mem_fp);
            // 判断是否为结束标记（姓名为空）
            if (temp_node.name[0] == '\0') {
                break;
            }
            // 分配新节点并拷贝数据
            clubmembernode* new_node = (clubmembernode*)malloc(sizeof(clubmembernode));
            if (new_node == NULL) {
                break;
            }
            memcpy(new_node, &temp_node, sizeof(clubmembernode));
            new_node->next = NULL;

            // 尾插法添加节点
            if (g_clubs[i].member_head == NULL) {
                g_clubs[i].member_head = new_node;
                tail = new_node;
            }
            else {
                tail->next = new_node;
                tail = new_node;


            }
        }
        fclose(mem_fp);
    }
    printf("【成功】社团数据已加载，共%d个社团！\n", g_club_count);
}
// -------------------------- 三、财务模块--------------------------
// 1. 财务流水：按社团名称筛选（返回符合条件的记录数，结果存入out_records）
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
// 2. 财务流水：新增记录（返回1成功，0失败：超出最大记录数/金额非正）
int finance_add(const financerecord* record) {
    if (record == NULL || g_finance_count >= MAX_RECORD || record->amount <= 0) {
        return 0;
    }
    memcpy(&g_finance_records[g_finance_count], record, sizeof(financerecord));
    g_finance_count++;
    return 1;
}
// 3. 财务流水：校验余额（按社团名称，返回当前余额，确保支出后余额非负）
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
// 4. 财务流水：删除记录（按索引，返回1成功，0失败：索引无效）
int finance_delete(int index) {
    if (index < 0 || index >= g_finance_count) {
        return 0;
    }
    // 数组前移覆盖
    for (int i = index; i < g_finance_count - 1; i++) {
        memcpy(&g_finance_records[i], &g_finance_records[i + 1], sizeof(financerecord));
    }
    g_finance_count--;
    return 1;
}
// 5. 财务数据保存到文件（二进制持久化，路径：finance.dat）
void finance_save_to_file() {
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
// 6. 财务数据从文件加载（二进制读取，路径：finance.dat）
void finance_load_from_file() {
    FILE* fp = fopen("finance.dat", "rb");
    if (fp == NULL) {
        printf("【提示】无历史财务数据，将创建新数据！\n");
        return;
    }
    fread(&g_finance_count, sizeof(int), 1, fp);
    if (g_finance_count > MAX_RECORD) {
        g_finance_count = MAX_RECORD;
    }
    fread(g_finance_records, sizeof(financerecord), g_finance_count, fp);
    fclose(fp);
    printf("【成功】财务数据已加载，共%d条流水记录！\n", g_finance_count);
}
// -------------------------- 四、加载入口 --------------------------
// 1. 所有数据保存到文件（用户+社团+财务）
void all_data_save() {
    user_save_to_file();
    club_save_to_file();
    finance_save_to_file();
    printf("【成功】所有数据已完成持久化保存！\n");
}
// 2. 所有数据从文件加载（用户+社团+财务）
void all_data_load() {
    user_load_from_file();
    club_load_from_file();
    finance_load_from_file();
    printf("【成功】所有历史数据已完成加载！\n");

}
