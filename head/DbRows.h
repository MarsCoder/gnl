/*
 * DbRows.h
 *
 *  Created on: May 6, 2015
 *      Author: mars
 */

#ifndef DBROWS_H_
#define DBROWS_H_

#include <vector>
#include <mysql/mysql.h>
#include "../head/DbRow.h"
namespace gnl {

class DbRows {
public:
	DbRows();
	DbRows(MYSQL_RES *my_res);
	virtual ~DbRows();
	//从查询的行记录集合中根据表字段名对应的字段值取出结果记录集合
	std::vector<const DbRow*>& get_rows(const char* name, const char* value);
	//取得所有的查询结果集的引用
	const std::vector<DbRow *>& get_rows() const;
	//取得所有的查询结果集
	std::vector<DbRow *> __get_rows() const;
	//返回 数据表字段名
	std::vector<const char*> get_name();
	//从查询的行记录集合中根据索引下标取得对应的某行记录
	const DbRow* operator[](unsigned int) const;
	//
	void operator=(MYSQL_RES*);
	//
	void operator=(DbRows*);
	//判断结果集是否为空
	bool empty();
	//返回纪律集的个数
	unsigned int length() const;

private:
	// 数据表字段名
	std::vector<const char*> names_;
	//查询结果行集合，其中的元素 db_row 是动态添加进去的
	std::vector<DbRow *> rows_;
	// 临时结果行集合
	std::vector<const DbRow *> rows_tmp_;
	//MYSQL_RES *my_res_;
};

} /* namespace gnl */

#endif /* DBROWS_H_ */
