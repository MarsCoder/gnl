/*
 * MsgQueue.h
 *
 *  Created on: May 21, 2015
 *      Author: mars
 */

#ifndef MSGQUEUE_H_
#define MSGQUEUE_H_

#include <string>

namespace gnl {

#define MAX_TEXT 1024

struct DMsqQueue
{
	long mtype;
	char text[MAX_TEXT];
};

class MsgQueue {
public:
	MsgQueue(std::string queueName, int flag);
	virtual ~MsgQueue();
	//接收消息
	bool receive(DMsqQueue& msgObject, int flag);
	//发送消息
	bool send(DMsqQueue& msgObject, int flag);
	//设置
	bool receive_timeout(DMsqQueue& msgObject, int timeout = 5);
	bool send_timeout(DMsqQueue& msgObject, int timeout = 5);
	bool remove(DMsqQueue& msgObject);

private:
	int msg_queue_id_;
};

} /* namespace gnl */

#endif /* MSGQUEUE_H_ */
