/*
 * ThreadPool.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: mars
 *      写完才发觉线程池本身好像是不可重入的。。。 <--__<--  (还未确定)
 */

#include <assert.h>
#include <algorithm>

#include "../head/ThreadPool1.h"


namespace gnl1 {

//空操作
void noop()
{

}

//第一个参数位线程池中线程的个数，第二个参数位线程池中任务队列缓冲区的的大小（默认0为无限大）
ThreadPool::ThreadPool(ssize_t Qsize, ssize_t Tsize = 0)
	:taskQueue(Tsize),threadNumber(Qsize),maxQueueSize(Tsize),running(true) {
	// TODO Auto-generated constructor stub
	assert(threads.empty());
	threads.reserve(threadNumber);	//一次性分配线程池内存空间
	for(int i=0; i<threadNumber; ++i){
		//将新创建的线程放进进程池
		threads.push_back(std::shared_ptr<std::thread>
				(new std::thread(std::bind(&ThreadPool::runInThread, this))));
	}

}

ThreadPool::~ThreadPool() {
	// TODO Auto-generated destructor stub
}


//线程的入口函数
void ThreadPool::runInThread()
{
	while(running){
		Task task = taskQueue.pop(); //取得任务，没有任务便阻塞在BlockingQueue缓冲队列中，等待被唤醒
		task();		//执行任务
	}
}


//添加任务函数
void ThreadPool::addTask(const Task& task)
{
	taskQueue.push(task);		//添加任务到任务队列
}

void ThreadPool::stop()
{
	running = false;
	//添加threadNumber个空任务，确保running为false后每个线程都被唤醒一次
	for(int i=0; i<threadNumber;  ++i){
		addTask(noop);
	}
	//等待每一个进程退出
	for(std::vector<std::shared_ptr<std::thread>>::iterator i = threads.begin();
				i != threads.end(); ++i){
			(*i)->join();
		}
}

} /* namespace gnl */
