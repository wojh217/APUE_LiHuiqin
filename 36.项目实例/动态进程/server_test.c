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

#define MINSPARESERVER 	5
#define MAXSPARESERVER 	10
#define MAXCLIENTS  	20
#define BUFSIZE     1024
#define IPSTRSIZE      40
#define LINEBUFSIZE    8

#define SIG_NOTIFY SIGUSR2

enum 
{
	STATE_IDEL = 0,
	STATE_BUSY
};

struct server_st
{
	pid_t pid;
	int state;
};

static struct server_st *serverpool;

static int idle_cnt = 0;
static int busy_cnt = 0;

static int sd;



static void usr2_handler(int sig)
{
	printf("receive signal\n");
	return ;
}

static void server_job(int pos)
{
	int ppid;
	socklen_t radde_len;
	struct sockaddr_in raddr;
	int client_sd;
	int len;
	time_t stamp;
	char ipstr[IPSTRSIZE];
	char linebuf[LINEBUFSIZE];

	ppid = getppid();
	// radde_len = sizeof(raddr);

	while(1)
	{
		serverpool[pos].state = STATE_IDEL;  // 每次新的accept前都是空闲
		kill(ppid, SIG_NOTIFY);

		client_sd = accept(sd, (void *)&raddr, &radde_len);
		if(client_sd < 0)
		{
			if(errno != EINTR || errno != EAGAIN)
			{
				perror("accept()");
				exit(1);
			}
			continue;
			
		}

		serverpool[pos].state = STATE_BUSY; // 每次新的accept后都是忙碌
		kill(ppid, SIG_NOTIFY);


		// memset(buf, 0, BUFSIZE);
		// inet_ntop(AF_INET, &raddr.sin_addr, buf, BUFSIZE);
		// printf("[%d]RECEIVE FROM %s:%d\n", getpid(), buf, ntohs(raddr.sin_port));
		// // printf("RECEIVE FROM %d\n", ntohs(raddr.sin_port));
		// memset(buf, 0, BUFSIZE);
		// // len = sprintf(buf, FMT_STAMP, (long long)time(NULL));

		// len = snprintf(buf, BUFSIZE, FMT_STAMP, time(NULL));


		stamp = time(NULL);
		len = snprintf(linebuf, LINEBUFSIZE, FMT_STAMP, stamp);

		send(client_sd, linebuf, len, 0);
		// {
		// 	perror("send");
		// 	exit(1);
		// }

		sleep(5);
		close(client_sd); // 服务端主动关闭
		

	}
}


static int add_1_server(void)
{
	int i, pid;
	
	// 不准，不要此判断
	// if(idle_cnt + busy_cnt >= MAXCLIENTS)
	// 	return -1;

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
		// idle_cnt++;
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
	idle_cnt--;

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
		if(kill(serverpool[i].pid, 0) < 0) // 这里应该用-1判断吧
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

	idle_cnt = idle;
	busy_cnt = busy;
	return 0;
}

int main()
{
	struct sigaction sa, osa;

	int val=1;
	struct sockaddr_in laddr;
	sigset_t set, oset;
	int i;

	
	sa.sa_handler = SIG_IGN; // 父进程wait收尸就是通过此信号
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_NOCLDWAIT; // 子进程不产生僵尸进程
	sigaction(SIGCHLD, &sa, &osa); // 键盘中断时，不产生僵尸进程

	sa.sa_handler = usr2_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIG_NOTIFY, &sa, &osa);


	// 屏蔽信号
	sigemptyset(&set);
	sigaddset(&set, SIG_NOTIFY);
	sigprocmask(SIG_BLOCK, &set, &oset);

	// 使用mmap申请内存
	serverpool = mmap(NULL, sizeof(struct server_st) * MAXCLIENTS, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); // 匿名映射
	if(serverpool == MAP_FAILED)
	{
		perror("mmap()");
		exit(1);
	}

	for(i = 0; i < MAXCLIENTS; i++)
	{
		serverpool[i].pid = -1;
	}

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
	// laddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
	{
		perror("bind()");
		exit(1);
	}


	if(listen(sd, 50) < 0)
	{
		perror("listen()");
		exit(1);
	}



	for(i = 0; i < MINSPARESERVER; i++)
	{
		add_1_server();
	}

	while(1)
	{
		printf("while...\n");
		// 每当有进程的状态变化时，判断是否需要增加或销毁进程, 状态变化用信号提示
		// 注意标准信号会发生丢失
		sigsuspend(&oset);


		// 查看进程池
		scan_pool();

		printf("Middle\n");

		if(idle_cnt > MAXSPARESERVER)
		{
			for(i = 0; i < idle_cnt - MAXSPARESERVER; i++)
				del_1_server();
		}
		if(idle_cnt < MINSPARESERVER)
		{
			for(i = 0; i < MINSPARESERVER - idle_cnt; i++)
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