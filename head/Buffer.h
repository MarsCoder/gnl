/*
 * Buffer.h
 *
 *  Created on: Apr 20, 2015
 *      Author: mars
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <vector>

#include "common.h"

namespace gnl {

/// +-------------------+------------------+------------------+
/// |    可写缓冲区（前）  |  可读缓冲区（数据） |  可写缓冲区（后）   |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
class Buffer {
private:
	int readerIndex;		//可写缓冲区指针
	int writerIndex;		//可读缓冲区指针
	std::vector<char>	buf;		//缓冲区

public:
	Buffer();
	virtual ~Buffer();
	int readFd(int fd);			//读取数据到buffer的缓冲区
	int writableBytes() const;	//返回缓冲区的空闲空间（后半部分）
	int readableBytes() const;	//返回缓冲区的数据长度
	void append(const char* data, int len);	//将数据存进缓冲区
	void makespace(int len);				//调整缓冲区容量
	struct gp parsePacket();				//解析消息并将消息返回

};

} /* namespace gnl */

#endif /* BUFFER_H_ */
