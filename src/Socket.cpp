/*
 * Socket.cpp
 *
 *  Created on: Apr 20, 2015
 *      Author: mars
 */

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include "../head/Socket.h"
#include "../head/common.h"
#include "../head/BlockingQueue.h"

//消息队列，网络层接收到的信息存放在这
extern gnl::BlockingQueue<struct user_data> inputMessage;

namespace gnl {

Socket::Socket():backlog(10),listenfd(-1),maxfd(-1) {
	// TODO Auto-generated constructor stub

}

Socket::~Socket() {
	// TODO Auto-generated destructor stub
}

/***************************************************
 * 		 	//初始化客户连接描述副
 ***************************************************/
void Socket::clientfdsInit()
{
	for(int i=0; i<FD_SETSIZE; ++i){
		clientfds[i] = -1;
	}
}

/***************************************************
 * 		 	//设置最大未完成连接的队列书
 ***************************************************/
void Socket::setBacklog(int log)
{
	backlog = log;
}

/***************************************************
 * 		 	//设置通信协议
 ***************************************************/
void Socket::setintFamily(int fam)
{
	servaddr.sin_family = fam;
}

/***************************************************
 * 		 	开始监听
 ***************************************************/
void Socket::startListen(short port)
{
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_port = htons(port);
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, INADDR, &servaddr.sin_addr);
	listenfd = socket(servaddr.sin_family, SOCK_STREAM, 0);
	if(::bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		perror("bind error:");
		exit(1);
	}
	listen(listenfd, backlog);
	do_select();
}

/***************************************************
 * 		 	处理i/o事件
 ***************************************************/
void Socket::do_select()
{
	int nready;
	clientfdsInit();	//初始化客户连接描述符
	FD_ZERO( &readSet );
	FD_SET(listenfd, &readSet);	//添加监听描述符
	maxfd = listenfd;
	for(;;){
		rset = readSet;
		//获取可用描述符的个数
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(listenfd, &rset)){
			newConnection();		//添加新连接
			if(--nready <=0){
				continue;
			}
		}
		handle_connection();	//处理连接的事件
	}

}

/***************************************************
 * 		 	//添加新连接
 ***************************************************/
void Socket::newConnection()
{
	int i;
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen;
	cliaddrlen = sizeof(cliaddrlen);
	if((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen)) == -1){
		if(errno == EINTR){
			return;
		}
		else{
			perror("acept error:");
			exit(1);
		}
	}
	std::cout<<"accetp a new client:" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port);
	//将连接描述符添加到描述符数组中
	for(i=0; i<FD_SETSIZE; ++i){
		if(clientfds[i] < 0){
			clientfds[i] = connfd;
			break;
		}
	}
	//如果连接多于FD_SETSIZE个，则出错
	if(i == FD_SETSIZE){
		std::cerr << "too many clients." << std::endl;
		exit(1);
	}
	//将新的描述符添加到描述符集合中
	FD_SET(connfd, &readSet);
	//记录最大描述符
	maxfd = (connfd > maxfd ? connfd : maxfd);
	buffer[connfd];		//为新连接添加一个缓冲区
	std::cout << "new connect " << std::endl;
}

/***************************************************
 * 		 	//处理连接
 ***************************************************/
void Socket::handle_connection()
{
	char buf[MAXLINE];
	memset(buf, 0, MAXLINE);
	for(int i=0; i<=FD_SETSIZE; ++i){
		if(clientfds[i] < 0)
			continue;
		//测试客户描述符是否准备好
		if(FD_ISSET(clientfds[i], &rset)){
			//接收客户端发送的信息
			int n = buffer[clientfds[i]].readFd(clientfds[i]);	//读出数据到缓冲区
			if(n == 0){
				close(clientfds[i]);				//关闭连接
				buffer.erase(clientfds[i]);		//清除连接的读缓冲区
				FD_CLR(clientfds[i], &readSet);	//从描述符集合中清除
				clientfds[i] = -1;
				continue;
			}
			//从连接的缓冲区取出消息放到阻塞队列（可能有多条）
			while(1){
				struct user_data msg;
				msg.fd = clientfds[i];
				//取出一条消息(若无消息，这返回的msg.head.len = 0;)
				msg.gpm = buffer[clientfds[i]].parsePacket();
				if(msg.gpm.head.len == 0){
					break;
				}
				inputMessage.push(msg);	//将消息放进阻塞队列
			}
		}
	}
}

} /* namespace gnl */
