/*
 * common.h
 *
 *  Created on: Apr 21, 2015
 *      Author: mars
 */

#ifndef HEAD_COMMON_H_
#define HEAD_COMMON_H_

#define MAXLINE	1024
#define MESSAGE_HEAD_LEN	10//sizeof(struct gph)	//消息体长度
#define	PORT	18088							//端口
#define	INIT_BUFFER_SIZE	1024				//缓冲区初始大小
#define INADDR	"127.0.0.1"						//地址

#define ADD	1	//加法
#define SUB	2	//减法
#define	ECHO	3	//回显
#define RES 99	//结果


typedef unsigned short 	ushort;
typedef unsigned char	uchar;

//通信协议消息头结构体
struct gph{
	ushort len;	//消息长度
	char	version[4];	//版本号
	uchar category;	//消息类型
	char	reserve[3];	//保留保留

};

//通信协议消息结构体
struct gp{
	struct gph	head;	//消息头
	char	msg[INIT_BUFFER_SIZE*10];		//消息体指针
};

//用于读写两个的两个方面传递参数
struct user_data
{
	int fd;				//文件描述符
	struct gp	gpm;	//通信协议消息结构体
};


#endif /* HEAD_COMMON_H_ */
