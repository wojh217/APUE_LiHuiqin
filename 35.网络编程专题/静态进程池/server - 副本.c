#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "proto.h"


#define BUFSIZE 1024
#define PRONUM  4

static void server_loop(int sd)
{
	socklen_t caddrlen;
	struct sockaddr_in client_addr;
	char buf[BUFSIZE];
	int len;
	int clientfd;

	caddrlen = sizeof(client_addr);
	while(1)
	{
		clientfd = accept(sd, (void *)&client_addr, &caddrlen);

		if(clientfd < 0)
		{
			perror("accept()");
			exit(1);
		}

		memset(buf, 0, BUFSIZE);
		inet_ntop(AF_INET, &client_addr.sin_addr, buf, BUFSIZE);
		printf("[%d]RECEIVE FROM %s:%d\n", getpid(), buf, ntohs(client_addr.sin_port));

		// printf("RECEIVE FROM %d\n", ntohs(client_addr.sin_port));


		memset(buf, 0, BUFSIZE);
		len = sprintf(buf, FMT_STAMP, (long long)time(NULL));

		if(send(clientfd, buf, len, 0) < 0)
		{
			perror("send");
			exit(1);
		}

		close(clientfd); // 服务端主动关闭
	}
}
int main()
{
	// 固定四个进程在跑
	int sd;
	struct sockaddr_in server_addr;

	pid_t pid;
	int i;
	int forkcnt=0;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	int val = 1;

	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(SERVERPORT));
	// inet_pton(AF_INET, "127.0.0.1", server_addr.sin_addr);
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if(bind(sd, (void *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind()");
		exit(1);
	}


	if(listen(sd, 50) < 0)
	{
		perror("listen()");
		exit(1);
	}

	for(i = 0; i < PRONUM; i++)
	{
		pid = fork();
		if(pid < 0) // 失败时要不要将成功fork的子进程收尸
		{
			perror("fork");
			break;
		}

		forkcnt++;

		if(pid == 0)
		{
			server_loop(sd);
			exit(0);
		}
	}


	for(i = 0; i < forkcnt; i++)
		wait(NULL);

	
	close(sd);
	exit(0);
}