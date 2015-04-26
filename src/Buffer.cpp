/*
 * Buffer.cpp
 *
 *  Created on: Apr 20, 2015
 *      Author: mars
 */

#include <sys/uio.h>
#include <string.h>
#include <netinet/in.h>
#include <algorithm>
#include <stdlib.h> //malloc

#include "../head/Buffer.h"

namespace gnl {

Buffer::Buffer()
	:readerIndex(0),writerIndex(0),buf(INIT_BUFFER_SIZE)
{
	// TODO Auto-generated constructor stub
}

Buffer::~Buffer() {
	// TODO Auto-generated destructor stub
}

/***************************************************
 * 		读取数据到buffer的缓冲区
 ***************************************************/
int Buffer::readFd(int fd)
{
	char externbuf[65536];	//
	struct iovec vec[2];	//
	const int writable = writableBytes();	//记录缓冲区的空闲空间（后半部分）
	vec[0].iov_base = &*buf.begin() + writerIndex;	//记录缓冲区的空闲空间（后半部分）的起始地址
	vec[0].iov_len = writable;
	vec[1].iov_base = externbuf;
	vec[1].iov_len = sizeof externbuf;
	//读数据并分别将数据存放到缓冲区的空闲空间（后半部分）和externbuf缓冲区（如果有足够多的数据的话）
	int n = readv(fd, vec, 2);
	if(n < 0){
		return -1;
	}
	else if(n <= writable){
		writerIndex += n;
	}
	else{
		writerIndex = buf.size();
		append(externbuf, n-writable);	//将数据存进缓冲区
	}

	return n;
}

/***************************************************
 * 		 	返回缓冲区的空闲空间（后半部分）
 ***************************************************/
int Buffer::writableBytes() const
{
	return buf.size() - writerIndex;
}

/***************************************************
 * 		 	返回缓冲区的数据长度
 ***************************************************/
int Buffer::readableBytes() const
{
	return writerIndex - readerIndex;
}

/***************************************************
 * 		 	将数据存进缓冲区
 ***************************************************/
void Buffer::append(const char* data, int len)
{
	if(writableBytes() < len){
		makespace(len);		//调整缓冲区容量
	}
	std::copy(data, data+len, &*buf.begin()+writerIndex); //将数据存储到缓冲区
	writerIndex += len;
}

/***************************************************
 * 		 	调整缓冲区容量
 ***************************************************/
void Buffer::makespace(int len)
{
	if(readerIndex + writableBytes() < len){	//如果缓冲区总的空闲空间小于len，则增大缓冲区
		buf.resize(writerIndex + len);
	}
	else{
		int readable = writerIndex - readerIndex;
		std::copy(&*buf.begin()+readerIndex,	//将缓冲区内数据移动到缓冲区起始地址
				  &*buf.begin()+writerIndex,
				  &*buf.begin());
		readerIndex = 0;					//更新读指针
		writerIndex = readable;				//更新写指针
	}
}

/***************************************************
 * 		 	//解析消息并将消息返回
 ***************************************************/
struct gp Buffer::parsePacket()
{
	struct gp msg;
	int len;
	memcpy(&len, &*buf.begin()+readerIndex, sizeof(ushort));
	//len = ntohs(len);
	if(len + MESSAGE_HEAD_LEN > readableBytes()){
		msg.head.len = 0;
		return msg;
	}
	msg.head.len = len;	//消息长度
	readerIndex += sizeof(msg.head.len);	//读指针向前移动
	memcpy(&(msg.head).version, &*buf.begin() + readerIndex, sizeof(msg.head.version));	//取出版本号
	readerIndex += sizeof(msg.head.version);	//读指针向前移动
	memcpy(&(msg.head).category, &*buf.begin() + readerIndex, sizeof(msg.head.version));	//取出消息类型
	readerIndex += sizeof(msg.head.category);	//读指针向前移动
	readerIndex += sizeof(msg.head.reserve);	//读指针向前移动
	//msg.msg = malloc(len + 1);					//分配内存空间
	memcpy(msg.msg, &*buf.begin() + readerIndex, len);	//取出消息体
	readerIndex += len;
	return msg;
}

} /* namespace gnl */
