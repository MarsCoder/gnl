/*
 * main.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: mars
 */


#include <iostream>
#include <unistd.h>

//#include "../head/ThreadPool.h"
//#include "../head/common.h"
//#include "../head/BlockingQueue.h"
//#include "../head/Socket.h"
#include "../head/DbRow.h"
#include "../head/DbRows.h"
#include "../head/DbMysql.h"
#include "../head/DbMysqlPool.h"
#include "../head/JsonDbRows.h"
#include "../head/ParseConfig.h"

void test_DbMysql();
void test_DbMysqlPool();
void test_JsonDbRows();
void test_ParseConfig();

int main(int argc, char **argv)
{
	//test_JsonDbRows();
	test_ParseConfig();
	return 0;
}

void test_JsonDbRows()
{
		gnl::DbMysql* mysql = new gnl::DbMysql("localhost", "test", "root", "608955");
		if(mysql->connect() == false){
			fprintf(stderr, "connect error!\n");
			delete mysql;
			return;
		}
		if(mysql->is_connect())
			printf("is connect\n");
		if(mysql->table_exits("student"))
			printf("table_exits\n");
		char* query = "select * from student;";
		mysql->sql_select(query);

		gnl::JsonDbRows *jrr = new gnl::JsonDbRows(mysql->get_result());
		std::string str = jrr->get_result_in_json();
		std::cout << str << std::endl;


		mysql->free_result();

		mysql->close();
		delete mysql;
		return;
}

void test_DbMysql()
{
		gnl::DbMysql* mysql = new gnl::DbMysql("localhost", "test", "root", "608955");
		if(mysql->connect() == false){
			fprintf(stderr, "connect error!\n");
			delete mysql;
			return;
		}
		if(mysql->is_connect())
			printf("is connect\n");
		if(mysql->table_exits("student"))
			printf("table_exits\n");
		char* query = "select * from student;";
		mysql->sql_select(query);

		const gnl::DbRows* rows =  mysql->get_result();

		const std::vector<const gnl::DbRow*>* myrow = mysql->get_rows("name", "sixth");
		std::vector<const gnl::DbRow*>::const_iterator it = (*myrow).begin();
		for(;it < (*myrow).end(); ++it){
			///////////为什么会出现二义性？////////////////////////
			std::cout << "value: " << (*(*it))[(unsigned int)0] << std::endl;
		}

		unsigned int n = rows->length();
		for(unsigned int i=0; i<n; ++i){
			const gnl::DbRow* row = (*rows)[i];
			for(int j=0; j<row->length(); ++j){
				std::cout << (*row)[j] << " ";
			}
			std::cout << std::endl;
		}
		mysql->free_result();

		mysql->close();
		delete mysql;
		return;
}

void test_DbMysqlPool()
{
	gnl::DbMysqlPool* pool = new gnl::DbMysqlPool("localhost", "test", "root", "608955");
	pool->set_idle(60);	//设置连接池中空闲连接的生存周期
	gnl::DbMysql* db1 = pool->peek("UTF-8");
	gnl::DbMysql* db2 = pool->peek();

	if(db1->connect() == false){
		fprintf(stderr, "connect error!\n");
		pool->put(db1);
		db1 = nullptr;
	}
	if(db1->is_connect())
		printf("is connect\n");
	if(db1->table_exits("student"))
		printf("table_exits\n");
	char* query = "select * from student;";
	db1->sql_select(query);
	db2->sql_select(query);


	const gnl::DbRows* rows1 = db1->get_result();
	const std::vector<gnl::DbRow*>* row2 = db2->get_rows();

	int n1 = rows1->length();
	for(int i=0; i < n1; ++i){
		const gnl::DbRow* row1 = (*rows1)[i];
		int rn1 = row1->length();
		for(int j=0; j<rn1; ++j){
			std::cout << (*row1)[j];
		}
		std::cout << std::endl;
	}
	db1->free_result();

	std::vector<gnl::DbRow*>::const_iterator it = row2->begin();
	for(; it != row2->end(); ++it){
		int rn2 = (*it)->length();
		for(int i=0; i< rn2; ++i){
			std::cout << (*it)->field_value(i);
		}
		std::cout << std::endl;
	}
	db2->free_result();

	pool->put(db1);
	pool->put(db2);
	fprintf(stderr, "finish first step!\n");

	db1 = pool->peek("UTF-8");
	db1->sql_select(query);
	rows1 = db1->get_result();

	n1 = rows1->length();
	for(int i=0; i < n1; ++i){
		const gnl::DbRow* row1 = (*rows1)[i];
		int rn1 = row1->length();
		for(int j=0; j<rn1; ++j){
			std::cout << (*row1)[j];
		}
		std::cout << std::endl;
	}
	db1->free_result();

	pool->put(db1);

	if(!pool)
		delete pool;

}

void test_ParseConfig()
{
	gnl::ParseConfig config("/home/mars/workspace/gnl/Debug/test.cfg");
	std::map<std::string, std::string>& m = config.get_map_config();
	std::map<std::string, std::string>::const_iterator mite = m.begin();
	for (; mite != m.end(); ++mite) {
		std::cout << mite->first << "=" << mite->second << std::endl;
		if(mite->second == "mars"){
			std::cout << "ffffffffffffffffffffff" << std::endl;
		}
	}

}
