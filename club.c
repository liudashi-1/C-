#include "club.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

 // 全局变量定义
clubinformation g_clubs[MAX_RECORD];
int g_club_count = 0;

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

int club_add(const clubinformation* club) {
    if (club == NULL || g_club_count >= MAX_RECORD) {
        return 0;
    }

    if (club_find_by_name(club->name) != -1) {
        return 0;
    }

    memcpy(&g_clubs[g_club_count], club, sizeof(clubinformation));
    g_clubs[g_club_count].member_head = NULL;
    g_club_count++;
    return 1;
}

int club_update(const char* club_name, const clubinformation* new_club) {
    if (club_name == NULL || new_club == NULL) {
        return 0;
    }

    int index = club_find_by_name(club_name);
    if (index == -1) {
        return 0;
    }

    g_clubs[index].member_head = g_clubs[index].member_head;
    memcpy(&g_clubs[index].name, new_club->name, sizeof(g_clubs[index].name));
    memcpy(&g_clubs[index].establish_time, new_club->establish_time,
        sizeof(g_clubs[index].establish_time));
    memcpy(&g_clubs[index].president_name, new_club->president_name,
        sizeof(g_clubs[index].president_name));
    return 1;
}

int club_delete(const char* club_name) {
    if (club_name == NULL || strlen(club_name) == 0) {
        return 0;
    }

    int index = club_find_by_name(club_name);
    if (index == -1) {
        return 0;
    }

    club_member_list_free(&g_clubs[index].member_head);

    for (int i = index; i < g_club_count - 1; i++) {
        memcpy(&g_clubs[i], &g_clubs[i + 1], sizeof(clubinformation));
    }
    g_club_count--;
    return 1;
}

void club_add_interactive(void) {
    if (g_club_count >= MAX_RECORD) {
        printf("【错误】社团数量已达上限！\n");
        return;
    }

    clubinformation new_club;
    memset(&new_club, 0, sizeof(clubinformation));

    printf("===== 新增社团 =====\n");

    // 输入社团名称
    printf("请输入社团名称（长度<%d）：", MAX_CLUB_NAME);
    scanf("%s", new_club.name);
    clear_input();

    // 检查社团是否已存在
    if (club_find_by_name(new_club.name) != -1) {
        printf("【错误】社团名称已存在！\n");
        return;
    }

    // 输入成立时间
    get_current_time(new_club.establish_time, 0);
    printf("社团成立时间：%s\n", new_club.establish_time);

    // 输入社长姓名
    printf("请输入社长姓名：");
    scanf("%s", new_club.president_name);
    clear_input();

    // 初始化成员链表
    new_club.member_head = NULL;

    // 添加社团
    if (club_add(&new_club)) {
        printf("【成功】社团新增成功！\n");
    }
    else {
        printf("【错误】社团新增失败！\n");
    }
}

void club_member_manage_interactive(void) {
    char club_name[MAX_CLUB_NAME];

    if (g_current_user.type == user_administrator) {
        printf("请输入要管理的社团名称：");
        scanf("%s", club_name);
        clear_input();
    }
    else {
        strcpy(club_name, g_current_user.club_name);
    }

    int club_idx = club_find_by_name(club_name);
    if (club_idx == -1) {
        printf("【错误】社团不存在！\n");
        return;
    }

    while (1) {
        clear_screen();
        printf("===== 社团成员管理 [%s] =====\n", club_name);
        printf("1. 查看成员列表\n");
        printf("2. 添加成员\n");
        printf("3. 修改成员职位\n");
        printf("4. 删除成员\n");
        printf("5. 返回\n");
        printf("==============================\n");

        int choice = input_int("请选择操作：");

        switch (choice) {
        case 1: {
            // 查看成员列表
            printf("\n成员列表：\n");
            clubmembernode* p = g_clubs[club_idx].member_head;
            int count = 0;
            while (p != NULL) {
                printf("%d. %s - %s\n", ++count, p->name,
                    memberposition2str(p->position));
                p = p->next;
            }
            if (count == 0) {
                printf("暂无成员\n");
            }
            wait_enter();
            break;
        }
        case 2: {
            // 添加成员
            char name[MAX_NAME];
            int position_choice;

            printf("请输入成员姓名：");
            scanf("%s", name);
            clear_input();

            printf("请选择职位：\n");
            printf("0 - 普通成员\n");
            printf("1 - 干事\n");
            printf("2 - 部长\n");
            position_choice = input_int("选择：");

            if (position_choice < 0 || position_choice > 2) {
                printf("【错误】无效的职位！\n");
                wait_enter();
                break;
            }

            if (club_member_add(&g_clubs[club_idx].member_head, name,
                (memberposition)position_choice)) {
                printf("【成功】成员添加成功！\n");
            }
            else {
                printf("【错误】成员已存在或添加失败！\n");
            }
            wait_enter();
            break;
        }
        case 3: {
            // 修改成员职位
            char name[MAX_NAME];
            int new_position_choice;

            printf("请输入成员姓名：");
            scanf("%s", name);
            clear_input();

            printf("请选择新职位：\n");
            printf("0 - 普通成员\n");
            printf("1 - 干事\n");
            printf("2 - 部长\n");
            new_position_choice = input_int("选择：");

            if (new_position_choice < 0 || new_position_choice > 2) {
                printf("【错误】无效的职位！\n");
                wait_enter();
                break;
            }

            if (club_member_update_position(g_clubs[club_idx].member_head,
                name, (memberposition)new_position_choice)) {
                printf("【成功】成员职位修改成功！\n");
            }
            else {
                printf("【错误】成员不存在！\n");
            }
            wait_enter();
            break;
        }
        case 4: {
            // 删除成员
            char name[MAX_NAME];

            printf("请输入要删除的成员姓名：");
            scanf("%s", name);
            clear_input();

            if (club_member_delete(&g_clubs[club_idx].member_head, name)) {
                printf("【成功】成员删除成功！\n");
            }
            else {
                printf("【错误】成员不存在！\n");
            }
            wait_enter();
            break;
        }
        case 5:
            return;
        default:
            printf("【错误】无效选项！\n");
            wait_enter();
        }
    }
}

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

int club_member_add(clubmembernode** head, const char* name, memberposition position) {
    if (head == NULL || name == NULL || strlen(name) == 0) {
        return 0;
    }

    if (club_member_find(*head, name) != NULL) {
        return 0;
    }

    clubmembernode* new_node = (clubmembernode*)malloc(sizeof(clubmembernode));
    if (new_node == NULL) {
        return 0;
    }

    strcpy(new_node->name, name);
    new_node->position = position;
    new_node->next = *head;
    *head = new_node;

    return 1;
}

int club_member_update_position(clubmembernode* head, const char* name,
    memberposition new_position) {
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

int club_member_delete(clubmembernode** head, const char* name) {
    if (head == NULL || *head == NULL || name == NULL || strlen(name) == 0) {
        return 0;
    }

    clubmembernode* p = *head;
    clubmembernode* pre = NULL;

    while (p != NULL && strcmp(p->name, name) != 0) {
        pre = p;
        p = p->next;
    }

    if (p == NULL) {
        return 0;
    }

    if (pre == NULL) {
        *head = p->next;
    }
    else {
        pre->next = p->next;
    }

    free(p);
    return 1;
}

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
    *head = NULL;
}

void club_save_to_file(void) {
    FILE* fp = fopen("clubs.dat", "wb");
    if (fp == NULL) {
        printf("【错误】社团基本数据保存失败，无法打开文件！\n");
        return;
    }

    fwrite(&g_club_count, sizeof(int), 1, fp);
    fwrite(g_clubs, sizeof(clubinformation), g_club_count, fp);
    fclose(fp);

    for (int i = 0; i < g_club_count; i++) {
        char file_name[64];
        sprintf(file_name, "club_%s_members.dat", g_clubs[i].name);
        FILE* mem_fp = fopen(file_name, "wb");
        if (mem_fp == NULL) {
            printf("【错误】社团[%s]成员数据保存失败！\n", g_clubs[i].name);
            continue;
        }

        clubmembernode* p = g_clubs[i].member_head;
        while (p != NULL) {
            fwrite(p, sizeof(clubmembernode), 1, mem_fp);
            p = p->next;
        }

        clubmembernode end_node = { 0 };
        fwrite(&end_node, sizeof(clubmembernode), 1, mem_fp);
        fclose(mem_fp);
    }

    printf("【成功】社团数据已保存，共%d个社团！\n", g_club_count);
}

void club_load_from_file(void) {
    FILE* fp = fopen("clubs.dat", "rb");
    if (fp == NULL) {
        printf("【提示】无历史社团数据，将创建新数据！\n");
        return;
    }

    fread(&g_club_count, sizeof(int), 1, fp);
    if (g_club_count > MAX_RECORD) {
        g_club_count = MAX_RECORD;
    }

    if (g_club_count > 0) {
        fread(g_clubs, sizeof(clubinformation), g_club_count, fp);
    }
    fclose(fp);

    for (int i = 0; i < g_club_count; i++) {
        club_member_list_free(&g_clubs[i].member_head);

        char file_name[64];
        sprintf(file_name, "club_%s_members.dat", g_clubs[i].name);
        FILE* mem_fp = fopen(file_name, "rb");
        if (mem_fp == NULL) {
            g_clubs[i].member_head = NULL;
            continue;
        }

        clubmembernode* tail = NULL;
        clubmembernode temp_node;

        while (1) {
            fread(&temp_node, sizeof(clubmembernode), 1, mem_fp);
            if (temp_node.name[0] == '\0') {
                break;
            }

            clubmembernode* new_node = (clubmembernode*)malloc(sizeof(clubmembernode));
            if (new_node == NULL) {
                break;
            }

            memcpy(new_node, &temp_node, sizeof(clubmembernode));
            new_node->next = NULL;

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