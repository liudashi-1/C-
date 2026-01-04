#ifndef AUTH_H
#define AUTH_H

#include "data.h"

 /**
  * @brief 用户登录验证
  * @param username 用户名
  * @param password 密码（明文）
  * @return 登录成功返回1，失败返回0
  */
int user_login(const char* username, const char* password);

/**
 * @brief 用户注册交互界面
 */
void user_register(void);

/**
 * @brief 用户查找（按用户名，返回索引，-1表示不存在）
 * @param username 用户名
 * @return 用户索引，-1表示不存在
 */
int user_find(const char* username);

/**
 * @brief 用户新增
 * @param user 用户信息指针
 * @return 1成功，0失败（用户名已存在/超出最大记录数）
 */
int user_add(const userinformation* user);

/**
 * @brief 用户修改（按用户名）
 * @param username 原用户名
 * @param new_user 新用户信息
 * @return 1成功，0失败（用户不存在）
 */
int user_update(const char* username, const userinformation* new_user);

/**
 * @brief 用户删除（按用户名）
 * @param username 用户名
 * @return 1成功，0失败（用户不存在）
 */
int user_delete(const char* username);

/**
 * @brief 用户数据保存到文件
 */
void user_save_to_file(void);

/**
 * @brief 用户数据从文件加载
 */
void user_load_from_file(void);

#endif // AUTH_H