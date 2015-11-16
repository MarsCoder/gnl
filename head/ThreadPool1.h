/*
 * TreadPool1.h
 *
 *  Created on: Apr 19, 2015
 *      Author: mars
 */

#ifndef HEAD_TREADPOOL1_H_
#define HEAD_TREADPOOL1_H_

#include <functional>
#include <vector>
#include <thread>
#include <memory>

#include "../head/BlockingQueue.h"

namespace gnl1 {

class ThreadPool
{
public:
	typedef std::function<void ()> Task;
	//typedef std::vector<std::shared_ptr<std::thread>>	SPVThread;
	ThreadPool(ssize_t Qsize, ssize_t Tsize);
	ThreadPool(ThreadPool& ) = delete;	//禁止拷贝构造
	virtual ~ThreadPool();
	void addTask(const Task& task);		//添加任务
	void stop();						//退出所有线程
	ThreadPool& operator=(ThreadPool& ) = delete;	//禁止赋直/拷贝

private:
	void runInThread();
	void take();

private:
	std::vector<std::shared_ptr<std::thread>> threads;	//线程池
	gnl::BlockingQueue<Task> taskQueue;	//任务阻塞队列
	ssize_t		threadNumber;		//线程池线程个数
	ssize_t		maxQueueSize;		//任务队列最大任务个数
	bool		running;			//线程运行状态



};

} /* namespace gnl */

#endif /* HEAD_TREADPOOL1_H_ */
