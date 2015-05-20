/*
 * DbRow.h
 *
 *  Created on: May 6, 2015
 *      Author: mars
 */

#ifndef DBROW_H_
#define DBROW_H_

#include <vector>

namespace gnl {

class DbRow {
public:
	DbRow(const std::vector<const char*>& n);
	virtual ~DbRow();
	// 从查询结果的记录行中取得对应下标的字段值
	const char* field_value(unsigned int num) const;
	// 从查询结果的记录行中取得对应下标的字段值
	const char* field_value(const char* name) const;
	// 从查询结果的记录行中取得对应下标的字段值
	const char* operator[](unsigned int num) const;
	//从查询结果的记录行中根据字段名取得相应的字段值
	const char* operator[](const char* name) const;
	//每一行的字段数
	int length() const;
	//
	void push_back(const char* value);

private:
	//用于存放数据表字段值
	const std::vector<const char*> &names_;
	//用于存放对应字段的值（一行）
	std::vector<const char*> values_;


};

} /* namespace gnl */

#endif /* DBROW_H_ */
