#include "data.h"
#include "auth.h"
#include "club.h"
#include "finance.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

 /**
  * @brief 登录界面
  * @return 登录成功返回1，失败返回0
  */
int login_ui(void) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("===== 社团管理系统 - 登录 =====\n");
    printf("请输入用户名：");
    scanf("%s", username);
    clear_input();

    printf("请输入密码：");
    scanf("%s", password);
    clear_input();

    if (user_login(username, password)) {
        printf("【成功】登录成功！欢迎 %s（%s）\n",
            username, usertype2str(g_current_user.type));
        return 1;
    }
    else {
        printf("【错误】用户名/密码错误！\n");
        return 0;
    }
}

/**
 * @brief 管理员菜单
 */
void admin_menu(void) {
    while (1) {
        clear_screen();
        printf("===== 管理员菜单 =====\n");
        printf("1. 用户管理\n");
        printf("2. 社团管理\n");
        printf("3. 财务审核\n");
        printf("4. 数据备份\n");
        printf("5. 退出\n");
        printf("======================\n");

        int choice = input_int("请选择操作：");

        switch (choice) {
        case 1: {
            // 用户管理
            printf("\n===== 用户管理 =====\n");
            printf("1. 新增用户\n");
            printf("2. 删除用户\n");
            printf("3. 查看所有用户\n");
            printf("4. 返回\n");
            printf("====================\n");

            int sub_choice = input_int("请选择：");
            switch (sub_choice) {
            case 1:
                user_register();
                wait_enter();
                break;
            case 2: {
                char username[MAX_USERNAME];
                printf("请输入要删除的用户名：");
                scanf("%s", username);
                clear_input();

                if (user_delete(username)) {
                    printf("【成功】用户删除完成！\n");
                }
                else {
                    printf("【错误】用户不存在！\n");
                }
                wait_enter();
                break;
            }
            case 3: {
                printf("\n===== 所有用户列表 =====\n");
                for (int i = 0; i < g_user_count; i++) {
                    printf("%d. %s - %s [%s]\n",
                        i + 1,
                        g_users[i].username,
                        usertype2str(g_users[i].type),
                        g_users[i].club_name);
                }
                wait_enter();
                break;
            }
            case 4:
                break;
            default:
                printf("【错误】无效选项！\n");
                wait_enter();
            }
            break;
        }
        case 2: {
            // 社团管理
            printf("\n===== 社团管理 =====\n");
            printf("1. 新增社团\n");
            printf("2. 删除社团\n");
            printf("3. 成员管理\n");
            printf("4. 查看所有社团\n");
            printf("5. 返回\n");
            printf("====================\n");

            int sub_choice = input_int("请选择：");
            switch (sub_choice) {
            case 1:
                club_add_interactive();
                wait_enter();
                break;
            case 2: {
                char club_name[MAX_CLUB_NAME];
                printf("请输入要删除的社团名称：");
                scanf("%s", club_name);
                clear_input();

                if (club_delete(club_name)) {
                    printf("【成功】社团删除完成！\n");
                }
                else {
                    printf("【错误】社团不存在！\n");
                }
                wait_enter();
                break;
            }
            case 3:
                club_member_manage_interactive();
                break;
            case 4: {
                printf("\n===== 所有社团列表 =====\n");
                for (int i = 0; i < g_club_count; i++) {
                    printf("%d. %s - 社长：%s（%s）\n",
                        i + 1,
                        g_clubs[i].name,
                        g_clubs[i].president_name,
                        g_clubs[i].establish_time);
                }
                wait_enter();
                break;
            }
            case 5:
                break;
            default:
                printf("【错误】无效选项！\n");
                wait_enter();
            }
            break;
        }
        case 3: {
            // 财务审核
            printf("\n===== 财务审核 =====\n");
            financerecord records[MAX_RECORD];

            // 获取所有流水
            int total_count = 0;
            for (int i = 0; i < g_club_count && total_count < MAX_RECORD; i++) {
                int count = finance_filter_by_club(g_clubs[i].name,
                    &records[total_count],
                    MAX_RECORD - total_count);
                total_count += count;
            }

            if (total_count == 0) {
                printf("无财务流水记录！\n");
                wait_enter();
                break;
            }

            printf("流水列表（共%d条）：\n", total_count);
            for (int i = 0; i < total_count; i++) {
                printf("%d. %s | %s | %.2f | %s | %s\n",
                    i + 1,
                    records[i].club_name,
                    financetype2str(records[i].type),
                    records[i].amount,
                    records[i].create_time,
                    records[i].description);
            }

            int del_idx = input_int("\n请输入要删除的流水序号（0=取消）：");
            if (del_idx > 0 && del_idx <= total_count) {
                // 查找原始索引
                for (int i = 0; i < g_finance_count; i++) {
                    if (memcmp(&g_finance_records[i], &records[del_idx - 1],
                        sizeof(financerecord)) == 0) {
                        if (finance_delete(i)) {
                            printf("【成功】流水删除完成！\n");
                        }
                        else {
                            printf("【错误】删除失败！\n");
                        }
                        break;
                    }
                }
            }
            wait_enter();
            break;
        }
        case 4: {
            // 数据备份
            all_data_save();
            wait_enter();
            break;
        }
        case 5:
            // 退出
            return;
        default:
            printf("【错误】无效选项！\n");
            wait_enter();
        }
    }
}

/**
 * @brief 社长菜单
 */
void president_menu(void) {
    while (1) {
        clear_screen();
        printf("===== 社长菜单 [%s] =====\n", g_current_user.club_name);
        printf("1. 社团成员管理\n");
        printf("2. 财务流水录入\n");
        printf("3. 查看财务余额\n");
        printf("4. 导出财务报表\n");
        printf("5. 查看社团信息\n");
        printf("6. 退出\n");
        printf("========================\n");

        int choice = input_int("请选择操作：");

        switch (choice) {
        case 1:
            club_member_manage_interactive();
            break;
        case 2:
            finance_add_interactive();
            wait_enter();
            break;
        case 3:
            finance_check_balance_interactive();
            wait_enter();
            break;
        case 4:
            finance_export_report_interactive();
            wait_enter();
            break;
        case 5: {
            char club_name[MAX_CLUB_NAME];
            strcpy(club_name, g_current_user.club_name);
            int idx = club_find_by_name(club_name);
            if (idx == -1) {
                printf("【错误】所属社团不存在！\n");
            }
            else {
                printf("===== %s 社团信息 =====\n", club_name);
                printf("成立时间：%s\n", g_clubs[idx].establish_time);
                printf("社长：%s\n", g_clubs[idx].president_name);

                // 统计成员数量
                int member_count = 0;
                clubmembernode* p = g_clubs[idx].member_head;
                while (p != NULL) {
                    member_count++;
                    p = p->next;
                }
                printf("成员数量：%d人\n", member_count);
            }
            wait_enter();
            break;
        }
        case 6:
            return;
        default:
            printf("【错误】无效选项！\n");
            wait_enter();
        }
    }
}

/**
 * @brief 普通成员菜单
 */
void member_menu(void) {
    while (1) {
        clear_screen();
        printf("===== 普通成员菜单 [%s] =====\n", g_current_user.club_name);
        printf("1. 查看所属社团信息\n");
        printf("2. 查看财务余额\n");
        printf("3. 查看社团成员\n");
        printf("4. 退出\n");
        printf("============================\n");

        int choice = input_int("请选择操作：");

        switch (choice) {
        case 1: {
            char club_name[MAX_CLUB_NAME];
            strcpy(club_name, g_current_user.club_name);
            int idx = club_find_by_name(club_name);
            if (idx == -1) {
                printf("【错误】所属社团不存在！\n");
            }
            else {
                printf("===== %s 社团信息 =====\n", club_name);
                printf("成立时间：%s\n", g_clubs[idx].establish_time);
                printf("社长：%s\n", g_clubs[idx].president_name);
            }
            wait_enter();
            break;
        }
        case 2:
            finance_check_balance_interactive();
            wait_enter();
            break;
        case 3: {
            char club_name[MAX_CLUB_NAME];
            strcpy(club_name, g_current_user.club_name);
            int idx = club_find_by_name(club_name);
            if (idx == -1) {
                printf("【错误】所属社团不存在！\n");
            }
            else {
                printf("===== %s 成员列表 =====\n", club_name);
                clubmembernode* p = g_clubs[idx].member_head;
                int count = 0;
                while (p != NULL) {
                    printf("%d. %s - %s\n", ++count, p->name,
                        memberposition2str(p->position));
                    p = p->next;
                }
                if (count == 0) {
                    printf("暂无成员\n");
                }
            }
            wait_enter();
            break;
        }
        case 4:
            return;
        default:
            printf("【错误】无效选项！\n");
            wait_enter();
        }
    }
}

/**
 * @brief 主函数 - 系统入口
 * @return 程序退出状态
 */
int main(void) {
    // 1. 加载历史数据
    all_data_load();

    // 2. 主循环
    while (1) {
        clear_screen();
        printf("===== 高校社团管理系统 =====\n");
        printf("1. 登录\n");
        printf("2. 注册\n");
        printf("3. 退出\n");
        printf("===========================\n");

        int choice = input_int("请选择操作：");

        switch (choice) {
        case 1: {
            // 登录
            if (login_ui()) {
                wait_enter();

                // 根据用户类型进入对应菜单
                while (1) {
                    if (g_current_user.type == user_administrator) {
                        admin_menu();
                    }
                    else if (g_current_user.type == user_president) {
                        president_menu();
                    }
                    else if (g_current_user.type == user_member) {
                        member_menu();
                    }

                    // 退出登录确认
                    printf("\n是否退出登录？（1=是，0=否）：");
                    int logout = input_int("");
                    if (logout == 1) {
                        memset(&g_current_user, 0, sizeof(userinformation));
                        break;
                    }
                }
            }
            else {
                wait_enter();
            }
            break;
        }
        case 2: {
            // 注册
            user_register();
            wait_enter();
            break;
        }
        case 3: {
            // 退出系统
            all_data_save();
            printf("【成功】数据已保存，退出系统！\n");
            return 0;
        }
        default:
            printf("【错误】无效选项！\n");
            wait_enter();
        }
    }

    return 0;
}