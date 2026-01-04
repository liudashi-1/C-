/**
 * @file finance.h
 * @brief 财务管理模块声明
 * @author 社团管理系统开发组
 * @date 2024
 */

#ifndef FINANCE_H
#define FINANCE_H

#include "data.h"

 /**
  * @brief 财务流水：按社团名称筛选
  * @param club_name 社团名称
  * @param out_records 输出记录数组
  * @param max_out 最大输出记录数
  * @return 符合条件的记录数
  */
int finance_filter_by_club(const char* club_name, financerecord* out_records, int max_out);

/**
 * @brief 财务流水：新增记录
 * @param record 财务记录指针
 * @return 1成功，0失败（超出最大记录数/金额非正）
 */
int finance_add(const financerecord* record);
float finance_calc_balance(const char* club_name);

/**
 * @brief 财务流水：删除记录（按索引）
 * @param index 记录索引
 * @return 1成功，0失败（索引无效）
 */
int finance_delete(int index);

/**
 * @brief 财务数据保存到文件
 */
void finance_save_to_file(void);

/**
 * @brief 财务数据从文件加载
 */
void finance_load_from_file(void);

/**
 * @brief 财务流水录入交互界面
 */
void finance_add_interactive(void);

/**
 * @brief 查看财务余额交互界面
 */
void finance_check_balance_interactive(void);

/**
 * @brief 导出财务报表交互界面
 */
void finance_export_report_interactive(void);

/**
 * @brief 所有数据保存到文件（用户+社团+财务）
 */
void all_data_save(void);

/**
 * @brief 所有数据从文件加载（用户+社团+财务）
 */
void all_data_load(void);

#endif // FINANCE_H