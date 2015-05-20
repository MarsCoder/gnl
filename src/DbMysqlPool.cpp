/*
 * DbMysqlPool.cpp
 *
 *  Created on: May 12, 2015
 *      Author: mars
 */

#include "../head/DbMysqlPool.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

namespace gnl {

DbMysqlPool::DbMysqlPool(const char* dbaddr, const char* dbname,
					const char* dbuser, const char* dbpass,
					int dblimit, unsigned long dbflags,
					bool auto_commit, int conn_timeout,
					int rw_timeout)

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
	if(dblimit > 0){
		dblimit_ = dblimit;
	}
	else{
		dblimit_ = 0;
	}
	dbcount_ = 0;
	ttl_ = -1;
	last_check_ = 0;
	check_inter_ = 30;
	set_id();

}

DbMysqlPool::~DbMysqlPool() {
	// TODO Auto-generated destructor stub
	if(dbaddr_)
		free (dbaddr_);
	if(dbname_)
		free(dbname_);
	if(dbuser_)
		free(dbuser_);
	if(dbpass_)
		free(dbpass_);
	std::list<DbMysql*>::iterator it = pool_.begin();
	for(; it != pool_.end(); ++it){
		delete (*it);
	}
}

//从数据库中连接池获得一个数据库连接，该函数返回的数据库
DbMysql* DbMysqlPool::peek(const char* charset)
{
	DbMysql* conn;
	mu_.lock();
	std::list<DbMysql*>::iterator it = pool_.begin();
	if(it != pool_.end()){
		conn = *it;
		pool_.erase(it);
		mu_.unlock();
		conn->set_when(time(nullptr));
		return conn;
	}
	else if(dbcount_ >= dblimit_){
		mu_.unlock();
		return nullptr;
	}
	++dbcount_;
	mu_.unlock();
	// 创建 mysql 连接句柄
	conn = create();
	/*
	static int __id = 0;
	//是否线程安全？ ////////////////////////////////////mark//////////////////////
	{
		std::unique_lock<std::mutex> lock(io_mu_);
		snprintf(id_, sizeof(id_), "id:%d", ++__id);
	}
	*/
	conn->set_id(id_);
	conn->set_when(time(nullptr));
	if(conn->connect("UTF-8") == true)
		return conn;
	//又不是线程安全？？////////////////////////////////mark///////////////////
	fprintf(stderr, "connect db failed!\n");
	put(conn, false);
	return nullptr;
}
//将数据库连接放回至连接池中
void DbMysqlPool::put(DbMysql* conn, bool keep)
{
	assert(conn);
	bool eq;
	const char* id = conn->get_id();
	if(id && strcmp(id, id_) == 0){
		eq = true;
	}
	else{
		eq = false;
	}
	time_t now = time(nullptr);
	mu_.lock();
	if(keep){
		conn->set_when(now);
		pool_.push_front(conn);
		// 如果该连接句柄不是由本连接池产生的，则需要
		// 重新设置连接句柄的 ID 标识
		if(!eq)
			conn->set_id(id_);
	}
	else{
		// 只有当该连接句柄由本连接池产生时，才需要减少记数器
		if(eq)
			--dbcount_;
		delete conn;
	}
	if(ttl_ >= 0 && now - last_check_ >= check_inter_){
		mu_.unlock();
		check_idle(ttl_);
		time(&last_check_);
		return;
	}
	mu_.unlock();

}
//当数据库连接的空闲时间大于等于ttl时,该连接将被释放；
//当 ttl 为 0 时则需要释放所有的数据库连接;
// 当 ttl 为 负数 时则不释放数据库连接;
int DbMysqlPool::check_idle(time_t ttl)
{
	if(ttl < 0)
		return 0;
	mu_.lock();
	if(pool_.empty()){
		mu_.unlock();
		return 0;
	}
	if(ttl == 0){
		int n = 0;
		std::list<DbMysql*>::iterator it = pool_.begin();
		for(; it != pool_.end(); ++it){
			delete *it;
			++n;
		}
		pool_.clear();
		dbcount_ = 0;
		mu_.unlock();
		return n;
	}
	int n = 0;
	time_t when, now=time(nullptr);
	std::list<DbMysql*>::iterator it, next;
	//为了减少遍历的次数，用反向迭代器
	std::list<DbMysql*>::reverse_iterator rit = pool_.rbegin();
	for(; rit != pool_.rend(); ){
		it = --rit.base();
		when = (*it)->get_when();
		if(when <=0 ){
			++rit;
			continue;
		}
		if(now - when < ttl){
			break;
		}

		delete *it;
		next = pool_.erase(it);
		rit = std::list<DbMysql*>::reverse_iterator(next);

		++n;
		--dbcount_;
	}
	mu_.unlock();
	return n;
}

//设置连接池中空闲连接的生存周期
DbMysqlPool& DbMysqlPool::set_idle(int idle)
{
	ttl_ = idle;
	return *this;
}
//设置自动检查空闲连接的时间间隔
DbMysqlPool& DbMysqlPool::set_check_inter(int n)
{
	check_inter_ = n;
	return *this;
}

//获得当前数据库连接池的最大连接数限制
int DbMysqlPool::get_dblimit() const
{
	return dblimit_;
}

//获得当前数据库连接池当前的连接数
int DbMysqlPool::get_dbcount() const
{
	return dbcount_;
}

//创建数据库连接句柄
DbMysql* DbMysqlPool::create()
{
	return new DbMysql(dbaddr_, dbname_, dbuser_,
		dbpass_, dbflags_, auto_commit_,
		conn_timeout_, rw_timeout_);
}

// 设置实例的唯一 ID 标识
void DbMysqlPool::DbMysqlPool::set_id()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	////////////非线程安全////////////////////////////////////////////////
	std::unique_lock<std::mutex> lock(io_mu_);
	snprintf(id_, sizeof(id_), "%d.%d.%d",
			 (int)tv.tv_sec, (int)tv.tv_usec, rand());
}

} /* namespace gnl */
