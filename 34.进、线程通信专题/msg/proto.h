#ifndef PROTO_H__
#define PROTO_H__

// KEYPROJ是一个整数
#define KEYPATH "/etc/services"
#define KEYPROJ 'g'

#define NAMESIZE 32

// msg结构体中必须包含long类型的type，为什么
struct msg_st 
{
	long mtype;
	char name[NAMESIZE];
	int math;
	int chinese;
};

#endif