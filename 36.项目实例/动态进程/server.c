#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>

#include "proto.h"

#define MINSPARESERVER 5  // 最少的空闲
#define MAXSPARESERVER 10 // 最大的空闲
#define MAXCLIENTS     20 // 连接的最大个数
#define IPSTRSIZE      40
#define LINEBUFSIZE    8


#define SIG_NOTIFY SIGUSR2 //SIGUSR21 SIGUSR2预留


enum {
	STATE_IDEL = 0,
	STATE_BUSY
};

struct server_st
{
	pid_t pid;
	int state;
	// int reuse; // 当某个进程使用超过固定次数后，进行销毁
};

static struct server_st *serverpool;

static int idle_count = 0, busy_count = 0;

static int sd;

static void usr2_handler(int s)
{
	// printf("receive signal\n");
	return ;
}

static void server_job(int pos)
{
	int ppid;
	struct sockaddr_in raddr;
	socklen_t raddr_len;
	int client_sd;
	int len;
	time_t stamp;
	char ipstr[IPSTRSIZE];
	char linebuf[LINEBUFSIZE];

	ppid = getppid();
	while(1)
	{
		serverpool[pos].state = STATE_IDEL; // 父进程已经设置过了，还需要设置吗？
		kill(ppid, SIG_NOTIFY); // 给父进程发送信号

		client_sd = accept(sd, (void*)&raddr, &raddr_len);
		if(client_sd < 0)
		{
			if(errno != EINTR || errno != EAGAIN)
			{
				perror("accept()");
				exit(1);

			}
			continue;
		}

		serverpool[pos].state = STATE_BUSY;
		kill(ppid, SIG_NOTIFY);

		inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
		// printf("[%d]client: %s: %d\n", getpid(), ipstr, ntohs(raddr.sin_port));

		stamp = time(NULL);
		len = snprintf(linebuf, LINEBUFSIZE, FMT_STAMP, stamp);

		//send校验
		send(client_sd, linebuf, len, 0);

		sleep(5);
		close(client_sd);

	}
}

static int add_1_server(void)
{
	int slot;
	int pid;


	if(idle_count + busy_count >= MAXCLIENTS)
		return -1;

	// 找空位
	for(slot = 0; slot < MAXCLIENTS; slot++)
	{
		if(serverpool[slot].pid == -1)
			break;
	}

	if(slot >= MAXCLIENTS)
		return -1;

	serverpool[slot].state = STATE_IDEL;
	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		return -1;
	}
	if(pid == 0)
	{
		server_job(slot);
		exit(0);
	}
	else
	{
		serverpool[slot].pid = pid;
		idle_count++;
	}

	return 0;


}

// 
static int del_1_server(void)
{
	int i;
	if(idle_count == 0)
		return -1;

	for(i = 0; i < MAXCLIENTS; i++)
	{
		if(serverpool[i].pid != -1 && serverpool[i].state == STATE_IDEL)
		{
			kill(serverpool[i].pid, SIGTERM); // 多个信号杀死要防止处理函数重入
			serverpool[i].pid = -1;
			idle_count--;
			break;
		}
	}

	return 0;
}



static int scan_pool(void)
{
	int idle=0, busy=0;
	int i;

	for(i = 0; i < MAXCLIENTS; i++)
	{
		if(serverpool[i].pid == -1)
			continue;
		if(kill(serverpool[i].pid, 0))
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
			//_exit(1);
			abort(); 
		}
	}

	idle_count = idle;
	busy_count = busy;
	return 0;
}
// 
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

	// 初始值设为-1
	for(i = 0; i < MAXCLIENTS; i++)
		serverpool[i].pid = -1;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	// reuse 端口
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}


	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);

	if(bind(sd, (void*)&laddr, sizeof(laddr)) < 0)
	{
		perror("bind()");
		exit(1);
	}

	if(listen(sd, 100) < 0)
	{
		perror("listen()");
		exit(1);
	}


	// 设置初始进程
	for(i = 0; i < MINSPARESERVER; i++)
	{
		add_1_server();
	}


	while(1)
	{
		sigsuspend(&oset);

		
		scan_pool();


		// control the poll
		if(idle_count > MAXSPARESERVER)
		{
			// 空闲太多，杀进程
			for(i = 0; i < (idle_count-MAXSPARESERVER); i++) // 杀或增加的过程中，进程数又发生了变化怎么办？
				del_1_server();
		}
		else if(idle_count < MINSPARESERVER)
		{
			// 空闲太少，增加进程
			for(i = 0; i < (MINSPARESERVER-idle_count); i++)
				add_1_server();
		}

		// 输出当前池状态
		for(i = 0; i < MAXCLIENTS; i++)
		{
			// printf("[%d],state:%d,fd:%d", i, serverpool[i].state, serverpool[i].pid);
			if(serverpool[i].pid == -1)
				putchar(' ');
			else if(serverpool[i].state == STATE_IDEL)
				putchar('.');
			else
				putchar('X');
		}
		putchar('\n');

	}

	sigprocmask(SIG_SETMASK, &oset, NULL);

	// unmmap()

	// while true; do (./client 127.0.0.1 &); sleep 1; done
	exit(0);

}



