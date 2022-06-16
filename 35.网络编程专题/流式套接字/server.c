#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "proto.h"

#define IPSTRSIZE 40
#define BUFSIZE   1024

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
	int sd, newsd;
	struct sockaddr_in laddr, raddr;
	socklen_t raddr_len;
	char ipstr[IPSTRSIZE];

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

	// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	raddr_len = sizeof(raddr);

	while(1)
	{
		// 回填地址
		newsd = accept(sd, (void *)&raddr, &raddr_len);
		if(newsd < 0)
		{
			perror("accept");
			exit(1);
		}
		inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
		printf("Client: %s:%d\n", ipstr, ntohs(raddr.sin_port));

		server_job(newsd);
		close(newsd); // 记得关闭
	}
	
	// 运行后可用nc 127.0.0.1 1989进行连接
	// 用ctr-c终止后，立马运行会提示bind失败，因为使用信号是异常终止，没有真正的释放


	close(sd);

	exit(0);
}


