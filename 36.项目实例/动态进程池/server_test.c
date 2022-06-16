#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>

#include "proto.h"

#define MINIDLE 	5
#define MAXIDLE 	10
#define MAXCLIENTS  	20
#define BUFSIZE     1024

#define SIG_NOTIFY SIGUSR1

static int idle_count = 0;
static int busy_count = 0;
static struct server_st *serverpool;
static int sd;

enum 
{
	STATE_IDEL,
	STATE_BUSY
};

struct server_st
{
	pid_t pid;
	int state;
};

static void sig_handler(int sig)
{
	return ;
}

static void server_job(int pos)
{
	socklen_t caddrlen;
	struct sockaddr_in client_addr;
	char buf[BUFSIZE];
	int len;
	int clientfd;
	int ppid;

	ppid = getppid();
	caddrlen = sizeof(client_addr);

	while(1)
	{
		serverpool[pos].state = STATE_IDEL;  // 每次新的accept前都是空闲
		kill(ppid, SIG_NOTIFY);

		clientfd = accept(sd, (void *)&client_addr, &caddrlen);
		if(clientfd < 0)
		{
			if(errno != EINTR || errno != EAGAIN)
			{
				perror("accept()");
				exit(1);
			}
			
		}

		serverpool[pos].state = STATE_BUSY; // 每次新的accept后都是忙碌
		kill(ppid, SIG_NOTIFY);


		memset(buf, 0, BUFSIZE);
		inet_ntop(AF_INET, &client_addr.sin_addr, buf, BUFSIZE);
		// printf("[%d]RECEIVE FROM %s:%d\n", getpid(), buf, ntohs(client_addr.sin_port));
		// printf("RECEIVE FROM %d\n", ntohs(client_addr.sin_port));
		memset(buf, 0, BUFSIZE);
		// len = sprintf(buf, FMT_STAMP, (long long)time(NULL));

		len = snprintf(buf, BUFSIZE, FMT_STAMP, time(NULL));

		if(send(clientfd, buf, len, 0) < 0)
		{
			perror("send");
			exit(1);
		}

		close(clientfd); // 服务端主动关闭
		sleep(5);

	}
}


static int add_1_server(void)
{
	int i, pid;
	
	if(idle_count + busy_count >= MAXCLIENTS)
		return -1;

	for(i = 0; i < MAXCLIENTS; i++)
	{
		if(serverpool[i].pid == -1)
			break;
	}

	if(i >= MAXCLIENTS)
		return -1;


	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}
	if(pid == 0)
	{
		server_job(i);
		exit(0);
	}
	else
	{
		serverpool[i].pid = pid;
		serverpool[i].state = STATE_IDEL;
		idle_count++;
	}
	
	// kill(getpid(), SIG_NOTIFY);

	return 0;

}

static int del_1_server(void)
{
	int i;

	for(i = 0; i < MAXCLIENTS; i++)
	{
		if(serverpool[0].pid != -1 && serverpool[0].state == STATE_IDEL)
			break;
	}

	if(i >= MAXCLIENTS)
		return -1;

	// 终止下标为i的进程
	kill(serverpool[i].pid, SIGTERM);
	serverpool[i].pid = -1;
	kill(getppid(), SIG_NOTIFY);
	idle_count--;

	return 0;

}

static int scan_pool(void)
{
	int i;
	int idle=0, busy=0;

	for(i = 0; i < MAXCLIENTS; i++)
	{
		if(serverpool[i].pid == -1)
			continue;
		if(kill(serverpool[i].pid, 0)) // 这里应该用-1判断吧
		{
			serverpool[i].pid = -1;
			continue;
		}
		if(serverpool[i].state == STATE_IDEL)
			idle++;
		else if(serverpool[i].state == STATE_BUSY)
			busy++;
		else
		{
			fprintf(stderr, "Unknown state.\n");
			abort();
		}
	}

	idle_count = idle;
	busy_count = busy;
	return 0;
}

int main()
{
	// 动态进程池
	sigset_t set, oset;
	struct sigaction act, oact;
	struct sockaddr_in server_addr;
	pid_t pid;
	int i;

	// 键盘中断时，不产生僵尸进程
	act.sa_handler = SIG_IGN; // 父进程wait收尸就是通过此信号
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NOCLDWAIT; // 子进程不产生僵尸进程
	sigaction(SIGCHLD, &act, &oact); // 键盘中断时，不产生僵尸进程

	// 注册信号处理函数
	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask); // 信号处理函数是否屏蔽其他信号
	act.sa_flags = 0;
	sigaction(SIG_NOTIFY, &act, &oact);

	// 阻塞SIG_NOTIFY信号
	sigemptyset(&set);
	sigaddset(&set, SIG_NOTIFY);
	sigprocmask(SIG_BLOCK, &set, &oset);


	// 申请内存
	serverpool = mmap(NULL, sizeof(struct server_st) * MAXCLIENTS, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); // 匿名映射
	if(serverpool == MAP_FAILED)
	{
		perror("mmap()");
		exit(1);
	}

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

	for(i = 0; i < MAXCLIENTS; i++)
	{
		serverpool[i].pid = -1;
	}

	for(i = 0; i < MINIDLE; i++)
	{
		add_1_server();
	}

	while(1)
	{
		// 每当有进程的状态变化时，判断是否需要增加或销毁进程, 状态变化用信号提示
		// 注意标准信号会发生丢失
		sigsuspend(&oset);


		// 查看进程池
		scan_pool();

		if(idle_count > MAXIDLE)
		{
			for(i = 0; i < (idle_count - MAXIDLE); i++)
				del_1_server();
		}
		else if(idle_count < MINIDLE)
		{
			for(i = 0; i < (MINIDLE - idle_count); i++)
				add_1_server();
		}

		// 输出当前状态
		for(i = 0; i < MAXCLIENTS; i++)
		{
			if(serverpool[i].pid == -1)
				putchar(' ');
			else if(serverpool[i].state == STATE_IDEL)
				putchar('.');
			else
				putchar('X');
		}

		putchar('\n');


	}


	// 在删除进程中自动杀掉子进程，且这里不需要收尸
	// for(i = 0; i < forkcnt; i++)
	// 	wait(NULL);

	
	close(sd);

	sigprocmask(SIG_SETMASK, &oset, NULL);

	exit(0);
}