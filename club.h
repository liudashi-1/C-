#ifndef CLUB_H
#define CLUB_H

#include "data.h"

 /**
  * @brief 社团查找（按社团名称，返回索引，-1表示不存在）
  * @param club_name 社团名称
  * @return 社团索引，-1表示不存在
  */
int club_find_by_name(const char* club_name);

/**
 * @brief 社团新增
 * @param club 社团信息指针
 * @return 1成功，0失败（名称已存在/超出最大记录数）
 */
int club_add(const clubinformation* club);

/**
 * @brief 社团修改（按社团名称）
 * @param club_name 原社团名称
 * @param new_club 新社团信息
 * @return 1成功，0失败（社团不存在）
 */
int club_update(const char* club_name, const clubinformation* new_club);

/**
 * @brief 社团删除（按社团名称）
 * @param club_name 社团名称
 * @return 1成功，0失败（社团不存在）
 */
int club_delete(const char* club_name);

/**
 * @brief 社团数据保存到文件
 */
void club_save_to_file(void);

/**
 * @brief 社团数据从文件加载
 */
void club_load_from_file(void);

/**
 * @brief 社团新增交互界面
 */
void club_add_interactive(void);

/**
 * @brief 社团成员管理交互界面
 */
void club_member_manage_interactive(void);

/**
 * @brief 成员链表：查找成员（按姓名）
 * @param head 链表头节点
 * @param name 成员姓名
 * @return 节点指针，NULL表示不存在
 */
clubmembernode* club_member_find(clubmembernode* head, const char* name);

/**
 * @brief 成员链表：新增成员（头插法）
 * @param head 链表头节点指针
 * @param name 成员姓名
 * @param position 成员职位
 * @return 1成功，0失败（成员已存在）
 */
int club_member_add(clubmembernode** head, const char* name, memberposition position);

/**
 * @brief 成员链表：修改成员职位（按姓名）
 * @param head 链表头节点
 * @param name 成员姓名
 * @param new_position 新职位
 * @return 1成功，0失败（成员不存在）
 */
int club_member_update_position(clubmembernode* head, const char* name, memberposition new_position);

/**
 * @brief 成员链表：删除成员（按姓名）
 * @param head 链表头节点指针
 * @param name 成员姓名
 * @return 1成功，0失败（成员不存在）
 */
int club_member_delete(clubmembernode** head, const char* name);

/**
 * @brief 成员链表：释放整个链表内存
 * @param head 链表头节点指针
 */
void club_member_list_free(clubmembernode** head);

#endif // CLUB_H