/*
 * Socket.h
 *
 *  Created on: Apr 20, 2015
 *      Author: mars
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <netinet/in.h>
#include <sys/select.h>

#include <map>
#include "../head/Buffer.h"

namespace gnl {

class Socket {
private:
	int backlog;	//最大未完成连接的队列书
	int listenfd;
	int clientfds[FD_SETSIZE];
	struct sockaddr_in servaddr;
	fd_set readSet;	//记录已经连接的全部描述符
	fd_set rset;	//用于记录每次select准备好的描述符
	int maxfd;		//最大描述符
	std::map<int,Buffer> buffer;	//用于保存每个连接的数据（缓存）


	void clientfdsInit();	//初始化客户连接描述副

public:
	Socket();
	virtual ~Socket();
	//int creatSocket();
	void setBacklog(int log);	//设置最大未完成连接的队列书
	void setintFamily(int fam);		//设置通信协议
	void startListen(short port);	//开始监听
	void do_select();
	void newConnection();		//添加新连接
	void handle_connection();	//处理连接
};

} /* namespace gnl */

#endif /* SOCKET_H_ */
