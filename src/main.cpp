/*
 * main.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: mars
 */


#include <iostream>
#include <unistd.h>

#include "../head/ThreadPool.h"
#include "../head/common.h"
#include "../head/BlockingQueue.h"
#include "../head/Socket.h"

std::mutex io_mu;
//消息队列，网络层接收到的信息存放在这
gnl::BlockingQueue<struct user_data> inputMessage;
//消息队列，将消息放进缓冲队列，等待网络层发送
gnl::BlockingQueue<struct user_data> outputMessage;

void add(struct user_data msg)
{
}

void sub(struct user_data msg)
{
}

void echo(struct user_data msg)
{
	outputMessage.push(msg);
	return;
}

void task(struct user_data msg)
{
	uchar msgCategory = msg.gpm.head.category;	//取出消息类型
	switch(msgCategory){
	case ADD:
		add(msg);
		break;
	case SUB:
		sub(msg);
		break;
	case ECHO:
		echo(msg);
		break;
	default:
		break;
	}
}

//负责将消息队列缓冲区的中的消息发送出去（符略了fd描述符可能无效的情况）
void writeMsg()
{
	int fd;
	struct user_data msg;
	struct gp *rmsg;
	while(1){
		msg = outputMessage.pop();
		fd = msg.fd;
		rmsg = &(msg.gpm);
		write(fd, rmsg, sizeof(struct gph)+rmsg->head.len);
	}
}

//负责将网络层接收到的每个消息封装成任务，然后将任务压进线程池的任务队列
void procTask(std::shared_ptr<gnl::ThreadPool> threadPool)
{
	struct user_data msg;
	while(1){
		msg = inputMessage.pop();	//取出一个消息
		msg.gpm.msg[msg.gpm.head.len] = '\0';
		std::cout << "receive a data:" << msg.gpm.msg << std::endl;
		threadPool->addTask(std::bind(task, msg));	//线线程池中添加一个任务
	}
}

int main(int argc, char **argv)
{
	gnl::Socket sock;
	std::shared_ptr<gnl::ThreadPool> threadPool(new gnl::ThreadPool(2,20));
	threadPool->start();
	//负责将消息队列缓冲区的中的消息发送出去
	std::thread t1(writeMsg);
	//负责将网络层接收到的每个消息封装成任务，然后将任务压进线程池的任务队列
	std::thread t2(procTask, threadPool);
	std::cout << "start sserver..." << std::endl;
	sock.startListen(PORT);

	t1.join();
	t2.join();
	threadPool->finish(); 		//退出线程
	std::cout << " all thread exit now! " << std::endl;
	return 0;
}

