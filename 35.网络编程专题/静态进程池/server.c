#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "proto.h"

#define IPSTRSIZE 40
#define BUFSIZE   1024
#define PROCNUM   4

static void server_loop(int sd);
static void server_job(int sd)
{

	// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
	char buf[BUFSIZE];
	int len;

	len = sprintf(buf, FMT_STAMP, (long long)time(NULL));

	// 这个地方能保证全部发送完吗？
	if(send(sd, buf, len, 0) < 0)
	{
		perror("send()");
		exit(1);
	}
}


int main()
{
	int sd;
	pid_t pid;
	int i;
	struct sockaddr_in laddr;
	
	sd = socket(AF_INET, SOCK_STREAM, 0); // 0,默认协议IPPROTO_TCP, 支持流式的还有IPPROTO_SCTP
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	// 设置端口可重新使用
	int val = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}


	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET, "0.0.0.0" , &laddr.sin_addr);

	if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
	{
		perror("bind()");
		exit(1);
	}

	// int listen(int sockfd, int backlog); // backlog原来是半连接大小，现在是全连接大小
	if(listen(sd, 200) < 0)
	{
		perror("listen()");
		exit(1);
	}


	for(i=0; i < PROCNUM; i++)
	{
		pid = fork();
		if(pid < 0)
		{
			perror("fork()");
			exit(1);
		}
		if(pid == 0)
		{
			server_loop(sd);
			exit(0);
		}
	}

	for(i=0; i < PROCNUM; i++)
		wait(NULL);

	close(sd);
	exit(0);
}

static void server_loop(int sd)
{
	struct sockaddr_in raddr;
	socklen_t raddr_len;
	int newsd;
	char ipstr[IPSTRSIZE];


	// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	raddr_len = sizeof(raddr);

	while(1)
	{
		// accept本身能实现互斥
		newsd = accept(sd, (void *)&raddr, &raddr_len);
		if(newsd < 0)
		{
			perror("accept");
			exit(1);
		}
		inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
		printf("[%d]Client: %s:%d\n", getpid(), ipstr, ntohs(raddr.sin_port));

		server_job(newsd);
		close(newsd); // 记得关闭
	}
	
	close(sd);

}



