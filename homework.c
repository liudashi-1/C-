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
// 2. 社团成员链表节点结构体
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
int main() {

    return 0;
}