/*
 * DbRow.cpp
 *
 *  Created on: May 6, 2015
 *      Author: mars
 */

#include "../head/DbRow.h"

#include <string.h>
#include <assert.h>

namespace gnl {

DbRow::DbRow(const std::vector<const char*>& name) : names_(name)
{
	// TODO Auto-generated constructor stub

}

DbRow::~DbRow() {
	// TODO Auto-generated destructor stub
}

// 从查询结果的记录行中取得对应下标的字段值
const char* DbRow::field_value(unsigned int num) const
{
	assert(num < values_.size());

	return values_[num];
}

// 从查询结果的记录行中取得对应下标的字段值
const char* DbRow::field_value(const char* name) const
{
	size_t   i, n = names_.size();

	// 必须保证表中字段名的个数与行记录的值的个数相等
	assert(names_.size() == values_.size());

	// 通过扫描字段名找出字段值的下标位置
	for (i = 0; i < n; i++)
	{
		if (strcasecmp(name, names_[i]) == 0)
			break;
	}
	if (i == n)
	{
		return (nullptr);
	}

	// 直接返回相应下标的字段值
	return (values_[i]);
}

// 从查询结果的记录行中取得对应下标的字段值
const char* DbRow::operator[](unsigned int num) const
{
	assert(num < values_.size());

	return values_[num];

}

//从查询结果的记录行中根据字段名取得相应的字段值
const char* DbRow::operator[](const char* name) const
{
	return field_value(name);
}

//每一行的字段数
int DbRow::length() const
{
	return values_.size();
}

//
void DbRow::push_back(const char* value)
{
	values_.push_back(value);
}
} /* namespace gnl */
