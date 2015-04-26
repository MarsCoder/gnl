/*
 * BlockingQueue.h
 *
 *  Created on: Apr 13, 2015
 *      Author: mars
 */

#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_

#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace gnl {

template<class T>
class BlockingQueue {
private:
	std::mutex mu;	//互斥量
	std::condition_variable empty;  //条件变量
	std::condition_variable full;	//条件变量
	std::deque<T> queBuf;	//缓冲区队列
	ssize_t	queueSize;	//队列缓冲区大小

public:
	BlockingQueue():queueSize(0){}	//无限缓冲区的队列
	BlockingQueue(unsigned int size):queueSize(size)
	{
		//queBuf.reserve(queueSize);	//分配内存空间
	}
	~BlockingQueue(){}

	void push(const T x){	//添加元素
		std::unique_lock<std::mutex> lock(mu);
		while(queBuf.size() >= queueSize && queueSize != 0){	//不是无限缓冲区队列且队列已满，则阻塞等待
			full.wait(lock);
		}
		queBuf.push_back(x);
		empty.notify_one();	//通知等待数据的线程
	}
	//
	T pop( ){		//取出元素元素
		std::unique_lock<std::mutex> lock(mu);
		while(queBuf.empty() ){		//队列为空，则阻塞等待
			empty.wait(lock);	//等待条件变量成立
		}
		T back = queBuf.front();
		queBuf.pop_front();
		full.notify_one();
		return back;
	}

};

} /* namespace gnl */

#endif /* BLOCKINGQUEUE_H_ */
