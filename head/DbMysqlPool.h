/*
 * DbMysqlPool.h
 *
 *  Created on: May 12, 2015
 *      Author: mars
 */

#ifndef MYSQLPOOL_H_
#define MYSQLPOOL_H_

#include "../head/DbMysql.h"
#include <mutex>
#include <condition_variable>

#include <list>

namespace gnl {

class DbMysqlPool {
public:
	DbMysqlPool(const char* dbaddr, const char* dbname,
		    const char* dbuser, const char* dbpass,
		    int dblimit = 64, unsigned long dbflags = 0,
		    bool auto_commit = true, int conn_timeout = 60,
		    int rw_timeout = 60);
	virtual ~DbMysqlPool();
	//从数据库中连接池获得一个数据库连接，该函数返回的数据库
	DbMysql* peek(const char* charset="UTF-8");
	//将数据库连接放回至连接池中
	void put(DbMysql* conn, bool keep=true);
	//当数据库连接的空闲时间大于等于ttl时,该连接将被释放；
	//当 idle 为 0 时则需要释放所有的数据库连接;
	int check_idle(time_t ttl);
	//设置连接池中空闲连接的生存周期
	DbMysqlPool& set_idle(int idle);
	//设置自动检查空闲连接的时间间隔
	DbMysqlPool& set_check_inter(int n);
	//获得当前数据库连接池的最大连接数限制
	int get_dblimit() const;
	//获得当前数据库连接池当前的连接数
	int get_dbcount() const;
	//创建数据库连接句柄
	DbMysql* create();

private:
	char* dbaddr_;	//数据库地址
	char* dbname_;	//数据库名字
	char* dbuser_;	//数据库用户名
	char* dbpass_;	//数据库密码
	unsigned long dbflags_;
	int conn_timeout_;	//连接超市
	int rw_timeout_;	//读写超时
	bool auto_commit_;	//自动提交事务

	std::list<DbMysql*> pool_;
	std::mutex mu_;		//互斥量（用于访问pool_对象）
	std::mutex io_mu_;	//用于io访问
	int dblimit_;		//连接池最大连接数
	int dbcount_;		// 当前已经打开的连接数
	char  id_[128];  	// 本类实例的唯一 ID 标识
	time_t ttl_;     	// 连接池中空闲连接被释放的超时值
	time_t last_check_;  // 上次检查空闲连接的时间截
	int    check_inter_; // 检查空闲连接的时间间隔

	// 设置实例的唯一 ID 标识
	void set_id();

};

} /* namespace gnl */

#endif /* MYSQLPOOL_H_ */
