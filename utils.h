#ifndef UTILS_H
#define UTILS_H

#include "data.h"

 /**
  * @brief 异或加密/解密函数
  * @param str 要加密/解密的字符串
  * @note 对称加密，加密解密同一逻辑
  */
void xor_encrypt_decrypt(char* str);

/**
 * @brief 获取当前时间字符串
 * @param time_str 时间字符串输出缓冲区
 * @param is_long 1=长格式（财务流水），0=短格式（社团成立时间）
 */
void get_current_time(char* time_str, int is_long);

/**
 * @brief 清理输入缓冲区
 */
void clear_input(void);

/**
 * @brief 清屏函数
 */
void clear_screen(void);

/**
 * @brief 获取整数输入
 * @param prompt 提示信息
 * @return 输入的整数
 */
int input_int(const char* prompt);

/**
 * @brief 获取浮点数输入
 * @param prompt 提示信息
 * @return 输入的浮点数
 */
float input_float(const char* prompt);

/**
 * @brief 等待用户按回车键
 */
void wait_enter(void);

/**
 * @brief 用户类型转字符串
 * @param type 用户类型
 * @return 对应的字符串描述
 */
const char* usertype2str(usertype type);

/**
 * @brief 财务类型转字符串
 * @param type 财务类型
 * @return 对应的字符串描述
 */
const char* financetype2str(financetype type);

/**
 * @brief 成员职位转字符串
 * @param position 成员职位
 * @return 对应的字符串描述
 */
const char* memberposition2str(memberposition position);

#endif // UTILS_H