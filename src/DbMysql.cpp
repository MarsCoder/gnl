/*
 * DbMysql.cpp
 *
 *  Created on: May 10, 2015
 *      Author: mars
 */

#include "../head/DbMysql.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

namespace gnl {

DbMysql::DbMysql(const char* dbaddr, const char* dbname,
					const char* dbuser,const char* dbpass,
					unsigned long dbflags, bool auto_commit,
					int conn_timeout, int rw_timeout) :
							my_res_(nullptr),
							conn_(nullptr),
							result_(nullptr),
							id_(nullptr)

{
	// TODO Auto-generated constructor stub
	assert(dbaddr && *dbaddr);
	assert(dbname && *dbname);
	//数据库地址
	dbaddr_ =(char*)malloc(strlen(dbaddr)+1);
	strcpy(dbaddr_, dbaddr);
	//数据库名
	dbname_ =(char*)malloc(strlen(dbname)+1);
	strcpy(dbname_, dbname);
	//数据库用户名
	if(dbuser){
		dbuser_ =(char*)malloc(strlen(dbuser)+1);
		strcpy(dbuser_, dbuser);
	}
	//数据库用户名密码
	if(dbpass){
		dbpass_ =(char*)malloc(strlen(dbpass)+1);
		strcpy(dbpass_, dbpass);
	}
	dbflags_ = dbflags;
	conn_timeout_ = conn_timeout;
	rw_timeout_ = rw_timeout;
	auto_commit_ =auto_commit;
	time(&when_);

}

DbMysql::~DbMysql() {
	// TODO Auto-generated destructor stub
	free(dbaddr_);
	free(dbname_);
	if(dbuser_)
		free(dbuser_);
	if(dbpass_)
		free(dbpass_);
	if(conn_)
		mysql_close(conn_);
	if(result_)
		delete result_;
	if(my_res_)
		mysql_free_result(my_res_);
	if(id_){
		free(id_);
	}
}

//连接数据库
bool DbMysql::connect(const char* local_charset)
{
	conn_ = mysql_init(nullptr);
	if(conn_ == nullptr){
		fprintf(stderr, "mysql init error!\n");
		return false;
	}
	//连接服务器超时
	if (conn_timeout_ > 0){
		mysql_options(conn_, MYSQL_OPT_CONNECT_TIMEOUT, &conn_timeout_);
	}
	//读写服务器超时
	if(rw_timeout_ > 0){
		mysql_options(conn_, MYSQL_OPT_READ_TIMEOUT, &rw_timeout_);
		mysql_options(conn_, MYSQL_OPT_WRITE_TIMEOUT, &rw_timeout_);
	}
	//设置连接丢失时重连
	bool reConnect = true;
	mysql_options(conn_, MYSQL_OPT_RECONNECT, &reConnect);
	//连接服务器
	//if(strchr(dbaddr_, ':'));
	//{

	//}
	if(mysql_real_connect(conn_, dbaddr_, dbuser_, dbpass_, dbname_, 0,
							nullptr, dbflags_) == nullptr){
		mysql_close(conn_);
		conn_ = nullptr;
		return false;
	}
	//设置默认的字符集
	if(local_charset){
		if(!mysql_set_character_set(conn_, local_charset)){
			fprintf(stderr, "set mysql charset error!\n");
		}
	}
	//设置自动提交autocommit模式
	if(mysql_autocommit(conn_, auto_commit_ ? true : false)){
		fprintf(stderr, "set autocommit error!\n");
		return false;
	}
	return true;
}
//判断数据库是否是连接状态
bool DbMysql::is_connect() const
{
	return conn_ ? true : false;
}

//关闭数据库连接
bool DbMysql::close()
{
	if(conn_ != nullptr){
		mysql_close(conn_);
		conn_ = nullptr;
	}
	return true;
}

//判断数据表是否存在
bool DbMysql::table_exits(const char* table_name)
{
	if(table_name == nullptr)
		return false;
	MYSQL_ROW row = nullptr;
	MYSQL_RES *res = mysql_list_tables(conn_, nullptr);
	if(res){
		while((row = mysql_fetch_row(res)) != nullptr){
			if(strcmp(table_name, row[0]) == 0)
				return true;
		}
		mysql_free_result(res);
	}
	return false;
}

//执行mysql语句
bool DbMysql::sane_mysql_query(const char* sql)
{
	if(conn_ == nullptr)
		return false;
	if(mysql_query(conn_, sql) == 0)
		return true;
	/* 重新打开MYSQL连接进行重试 */
	close();
	if (connect("UTF-8") == false)
	{
		return false;
	}
	if (mysql_query(conn_, sql) == 0)
		return true;
	close();
	return false;
}

//执行sql语句
bool DbMysql::sql_select(const char* sql)
{
	if(sane_mysql_query(sql) == false)
		return false;
	my_res_ = mysql_store_result(conn_);
	if(my_res_ == nullptr)
		return false;
	my_ulonglong nrow = mysql_num_rows(my_res_);
	if (nrow <= 0)
	{
		mysql_free_result(my_res_);
		my_res_ = nullptr;
		if(result_){
			delete result_;
			result_ = nullptr;
		}
		return true;
	}
	result_ = new DbRows(my_res_);
	return true;
}

//上次 sql 操作影响的记录行数
int DbMysql::affected_rows()
{
	if(!is_connect()){
		return -1;
	}
	return (int)mysql_affected_rows(conn_);
}

//获得执行 SQL 语句后的结果
const DbRows* DbMysql::get_result()
{
	return result_;
}

//从查询的行记录集合中根据表字段名对应的字段值取出结果记录集合
const std::vector<const DbRow*>* DbMysql::get_rows(const char* name, const char* value)
{
	if(result_ == nullptr)
		return nullptr;
	std::vector<const DbRow*>& rows = result_->get_rows(name, value);
	return &rows;
}

// 取得所有的查询结果集
const std::vector<DbRow*>* DbMysql::get_rows()
{
	if(result_ == nullptr)
		return nullptr;
	const std::vector<DbRow*>& rows = result_->get_rows();
	return &rows;
}

//释放上次查询的结果，当查询完成后，必须调用该函数来释放
void DbMysql::free_result()
{
	if(result_ != nullptr){
		delete result_;
		result_ = nullptr;
	}

	if(my_res_ != nullptr){
		mysql_free_result(my_res_);
		my_res_ = nullptr;
	}
}

//获得某个对应下标值的行记录
const DbRow* DbMysql::operator[](unsigned int num) const
{
	if(result_ == nullptr){
		return nullptr;
	}
	if(num >= length()){
		return nullptr;
	}
	return (*result_)[num];

}

//取得查询(sql_query)结果的行记录数
unsigned int DbMysql::length() const
{
	if(result_ == nullptr){
		return 0;
	}
	return (result_->length());
}
//查询(sql_query)执行完后结果是否为空
bool DbMysql::empty()
{
	return (length()==0 ? true : false);
}

//设置本实例的唯一 ID
DbMysql& DbMysql::set_id(const char* id)
{
	if(id == nullptr || *id == 0){
		return *this;
	}
	if(id_){
		free(id_);
	}
	id_ = (char*) malloc(strlen(id)+1);
	strcpy(id_, id);

	return *this;
}

//获得本实例的唯一 ID
const char* DbMysql::get_id()
{
	return id_;
}

//设置本数据库连接句柄当前被使用的时间
DbMysql& DbMysql::set_when(time_t now)
{
	when_ = now;
	return *this;
}
//获得该连接句柄上次被使用的时间
time_t DbMysql::get_when()
{
	return when_;
}

} /* namespace gnl */
