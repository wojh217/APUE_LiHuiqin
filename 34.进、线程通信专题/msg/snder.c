#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "proto.h"

int main()
{
	key_t key;
	int msgid;
	struct msg_st sbuf;

	key = ftok(KEYPATH, KEYPROJ); //创建key
	if(key < 0)
	{
		perror("ftok()");
		exit(1);
	}

	msgid = msgget(key, 0); // receive端先创建，需要有IPC_CREAT权限， 发送方不需要
	if(msgid < 0)
	{
		perror("msgget()");
		exit(1);
	}

	// 发送数据
	sbuf.mtype = 1;
	strcpy(sbuf.name, "Alan");
	sbuf.math = rand()%100;
	sbuf.chinese = rand()%100;

	if(msgsnd(msgid, &sbuf, sizeof(sbuf) - sizeof(long), 0) < 0)
	{
		perror("msgsnd()");
		exit(1);
	}

	// msgctl(); //不需要销毁,是由receive申请的

	exit(0);

}