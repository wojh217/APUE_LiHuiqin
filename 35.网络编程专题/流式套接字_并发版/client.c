#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "proto.h"

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
	int sd;
	struct sockaddr_in raddr;
	long long stamp;
	FILE *fp;
	char rbuf[BUFSIZE];
	int len;

	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET, argv[1], &raddr.sin_addr);

	if(connect(sd, (void*)&raddr, sizeof(raddr)) < 0)
	{
		perror("connect()");
		exit(1);
	}

	
	// ssize_t recv(int sockfd, void *buf, size_t len, int flags);
	
	// recv();
	// close();

	// 也可以用文件描述符来操作，将其封装成一个FILE流
	// udp可以封装成一个流文件吗?
	fp = fdopen(sd, "r+");
	if(fp == NULL)
	{
		perror("fdopen()");
		exit(1);
	}

	if(fscanf(fp, FMT_STAMP, &stamp) < 1)
		fprintf(stderr, "Bad format!\n");
	else
		fprintf(stdout, "stamp = %lld\n");
		fclose(fp);

	// 可以实现http协议的编码
	// fprintf(fp, "GET /test.jpg\r\n\r\n");
	// fflush(fp);

	// while(1)
	// {
	// 	len = fread(fbuf, 1, BUFSIZE, fp);
	// 	if(len < = 0) // 服务端主动关闭，则读取到0
	// 		break;
	// 	fwrite(rbuf, 1, len, stdout); // 将内容读取到显示终端
	// }
	

	exit(1);
}

