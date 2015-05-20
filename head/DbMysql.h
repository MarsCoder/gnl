/*
 * DbMysql.h
 *
 *  Created on: May 10, 2015
 *      Author: mars
 */

#ifndef DBMYSQL_H_
#define DBMYSQL_H_

#include "../head/DbRows.h"
#include <time.h>

namespace gnl {

class DbMysql {
public:
	DbMysql(const char* dbaddr, const char* dbname,
			 const char* dbuser,const char* dbpass,
			 unsigned long dbflags=0, bool auto_commit=true,
			 int conn_timeout=60, int rw_timeout=60);
	virtual ~DbMysql();
	//连接数据库
	bool connect(const char* local_charset= "UTF-8");
	//判断数据库是否是连接状态
	bool is_connect() const;
	//关闭数据库连接
	bool close();
	//判断数据表是否存在
	bool table_exits(const char* table_name);
	//执行sql语句（真正的执行sql语句）
	bool sane_mysql_query(const char* sql);
	//执行sql语句（调用sane_mysql_query，并将结果保存到本地）
	bool sql_select(const char* sql);
	//上次 sql 操作影响的记录行数
	int affected_rows();
	//获得执行 SQL 语句后的结果
	const DbRows* get_result();
	//从查询的行记录集合中根据表字段名对应的字段值取出结果记录集合
	const std::vector<const DbRow*>* get_rows(const char* name, const char* value);
	// 取得所有的查询结果集
	const std::vector<DbRow*>* get_rows();
	//释放上次查询的结果，当查询完成后，必须调用该函数来释放
	void free_result();
	//获得某个对应下标值的行记录
	const DbRow* operator[](unsigned int num) const;
	//取得查询(sql_query)结果的行记录数
	unsigned int length() const;
	//查询(sql_query)执行完后结果是否为空
	bool empty();
	//设置本实例的唯一 ID
	DbMysql& set_id(const char* id);
	//获得本实例的唯一 ID
	const char* get_id();
	//设置本数据库连接句柄当前被使用的时间
	DbMysql& set_when(time_t now);
	//获得该连接句柄上次被使用的时间
	time_t get_when();

private:
	char* dbaddr_;  // 数据库监听地址
	char* dbname_;  // 数据库名
	char* dbuser_;  // 数据库账号
	char* dbpass_;  // 数据库账号密码

	MYSQL_RES* my_res_;
	MYSQL* conn_;
	// 临时结果对象
	DbRows* result_;

	unsigned long dbflags_;
	int conn_timeout_;	//连接超市
	int rw_timeout_;	//读写超时
	bool auto_commit_;	//自动提交事务

	char* id_;	//实例 ID
	time_t when_;	//该数据库连接句柄最近被使用的时间
};

} /* namespace gnl */

#endif /* DBMYSQL_H_ */
