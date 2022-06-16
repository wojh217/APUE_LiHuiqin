#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "proto.h"

int main(int argc, char *argv[])
{
	int sd;
	int size;
	struct msg_st *sbufp;
	struct sockaddr_in raddr;

	if(argc < 3)
	{
		fprintf(stderr, "Usage: ./snder addr name\n");
		exit(1);
	}

	// 在NAMEMAX
	// 输入的name名字长度，不能超过NAMEMAX，也就是ARRAYMAX - 1
	if(strlen(argv[2]) > ARRAYMAX - 1)
	{
		fprintf(stderr, "name is too long");
		exit(1);
	}

	// 申请的大小随发送数据的大小而变化
	size = sizeof(struct msg_st) + strlen(argv[2]);
	printf("size: %d\n", size);
	sbufp = malloc(size);  // 需要申请的总大小
	if(sbufp == NULL)
	{
		perror("malloc()");
		exit(1);
	}

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	strcpy(sbufp->name, argv[2]);  // 这里应该用argv[2]吧


	sbufp->math = htonl(rand()%100);
	sbufp->chinese = htonl(rand()%100);

	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, argv[1], &raddr.sin_addr);


	if(sendto(sd, sbufp, size, 0, (void *)&raddr, sizeof(raddr)) < 0 )
	{
		perror("sendto");
		exit(1);
	}

	puts("OK!");


	close(sd);

	exit(0);
}