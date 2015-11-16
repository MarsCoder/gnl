/*
 * MsgQueue.cpp
 *
 *  Created on: May 21, 2015
 *      Author: mars
 */

#include "../head/MsgQueue.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

namespace gnl {


MsgQueue::MsgQueue(std::string queueName, int flag) {
	// TODO Auto-generated constructor stub
	std::string strFile = "/tmp/";
	strFile += queueName;
	std::string strCmd = "touch ";
	strCmd += strFile;
	system(strCmd.c_str());
	key_t mqkey = ftok(strFile.c_str(), 88);
	msg_queue_id_ = msgget(mqkey, flag);
}

MsgQueue::~MsgQueue() {
	// TODO Auto-generated destructor stub
}

//接收消息

bool MsgQueue::receive(DMsqQueue& msgObject, int flag)
{
	if(0 < msgrcv(msg_queue_id_, (void*)&msgObject,
			MAX_TEXT, msgObject.mtype, flag)){
		return true;
	}
	else{
		return false;
	}
}

//发送消息

bool MsgQueue::send(DMsqQueue& msgObject, int flag)
{
	if(0 < msgsnd(msg_queue_id_, (void*)&msgObject, MAX_TEXT, flag)){
		return true;
	}
	else{
		return false;
	}
}


bool MsgQueue::remove(DMsqQueue& msgObject)
{
	  if(!msgctl(msg_queue_id_, IPC_RMID, NULL))
	  {
	    return false;
	  }

	  return true;
}

} /* namespace gnl */
