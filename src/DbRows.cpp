/*
 * DbRows.cpp
 *
 *  Created on: May 6, 2015
 *      Author: mars
 */

#include "../head/DbRows.h"
#include "../head/DbRow.h"
#include <string.h>

namespace gnl {

DbRows::DbRows()//:my_res_(nullptr)
{
	// TODO Auto-generated constructor stub

}

DbRows::DbRows(MYSQL_RES* res)//:my_res_(nullptr)
{
	if(res == nullptr)
		return;
	int ncolumn = mysql_num_fields(res);
	MYSQL_FIELD *fields = mysql_fetch_field(res);
	// 取出变量名
	for(int i=0; i<ncolumn; ++i){
		names_.push_back(fields[i].name);
	}
	//取出所有行数据结果，加入动态数组中
	while(1){
		MYSQL_ROW my_row = mysql_fetch_row(res);
		if(my_row == nullptr)
			break;
		DbRow *row = new DbRow(names_);
		for(int i=0; i<ncolumn; ++i){
			row->push_back(my_row[i]);
		}
		rows_.push_back(row);
	}

}

DbRows::~DbRows() {
	// TODO Auto-generated destructor stub
	std::vector<DbRow*>::iterator it = rows_.begin();
	for(; it < rows_.end(); ++it){
		delete (*it);
	}
	/*
	if(my_res_){
		mysql_free_result(my_res_);
	}
	*/
}

//从查询的行记录集合中根据表字段名对应的字段值取出结果记录集合
std::vector<const DbRow*>& DbRows::get_rows(const char* name, const char* value)
{
	// 先清空上一次的临时结果集
	rows_tmp_.clear();
	if(empty()){
		return rows_tmp_;
	}
	int icolumn, ncolumn = names_.size();
	// 通过扫描字段名找出字段值的下标位置
	for(icolumn = 0; icolumn < ncolumn; ++icolumn){
		if(strcasecmp(name, names_[icolumn]) == 0)
			break;
	}
	const DbRow* row = nullptr;
	const char* ptr = nullptr;

	int nrow = rows_.size();
	// 比较对应下标相同的字段值的行记录
	for(int irow = 0; irow < nrow; ++irow){
		row = rows_[irow];
		ptr = (*row)[icolumn];
		if(strcmp(ptr, value) == 0){
			rows_tmp_.push_back(row);
		}
	}

	return rows_tmp_;
}

//返回记录集合的引用
const std::vector<DbRow*>& DbRows::get_rows() const
{
	return rows_;
}
//返回记录集合
std::vector<DbRow *> DbRows::__get_rows() const
{
	return rows_;
}

//返回 数据表字段名
std::vector<const char*> DbRows::get_name()
{
	return names_;
}

//从查询的行记录集合中根据索引下标取得对应的某行记录
const DbRow* DbRows::operator [](unsigned int num) const
{
	if(num >= rows_.size()){
		return nullptr;
	}
	DbRow *row = rows_[num];
	return row;
}

//
void DbRows::operator =(MYSQL_RES *res)
{
	if(res == nullptr)
		return;
	int ncolumn = mysql_num_fields(res);
	MYSQL_FIELD *fields = mysql_fetch_field(res);
	// 取出变量名
	for(int i=0; i<ncolumn; ++i){
		names_.push_back(fields[i].name);
	}
	//取出所有行数据结果，加入动态数组中
	while(1){
		MYSQL_ROW my_row = mysql_fetch_row(res);
		if(my_row == nullptr)
			break;
		DbRow *row = new DbRow(names_);
		for(int i=0; i<ncolumn; ++i){
			row->push_back(my_row[i]);
		}
		rows_.push_back(row);
	}
	//my_res_ = res;

}

//
void DbRows::operator =(DbRows* newRows)
{
	this->names_ = newRows->get_name();
	this->rows_ = newRows->__get_rows();
}
//判断结果集是否为空
bool DbRows::empty()
{
	return rows_.empty();
}

//返回纪律集的个数
unsigned int DbRows::length() const
{
	return (rows_.size());
}

} /* namespace gnl */
