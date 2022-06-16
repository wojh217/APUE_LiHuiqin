#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>

#define PROCNUM  20
#define LINESIZE 1024
#define FILENAME "/tmp/out"

//多进程打开文件，(使用信号量数组)取数加一重新写入

static int semid;

static void P(void)
{
	struct sembuf op; // 目前只有一个元素

	op.sem_num = 0;
	op.sem_op = -1; // 资源量减一操作
	op.sem_flg = 0;

	while(semop(semid, &op, 1) < 0) // 存在假错
	{
		if(errno != EINTR || errno != EAGAIN)
		{
			perror("semop()");
			exit(1);
		}
	}


}

static void V(void)
{
	struct sembuf op; 

	op.sem_num = 0;
	op.sem_op = 1; // 资源量加一操作
	op.sem_flg = 0;

	if(semop(semid, &op, 1) < 0)
	{
		perror("semop()");
		exit(1);
	}


}

static void func_add(void)
{
	FILE *fp;
	int fd;
	char linebuf[LINESIZE];

	fp = fopen(FILENAME, "r+");
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	// 不使用文件锁，而使用P、V操作
	// fd = fileno(fp);
	// lockf(fd, F_LOCK, 0);

	P();
	fgets(linebuf, LINESIZE, fp);
	fseek(fp, 0, SEEK_SET);
	sleep(1);
	fprintf(fp, "%d\n", atoi(linebuf) +1);
	fflush(fp);

	// 不使用文件锁，而使用P、V操作
	// lockf(fd, F_ULOCK, 0); // 释放文件锁
	V();

	fclose(fp);
	return ;
}

int main()
{

	int i;
	pid_t pid;
	// key = ftok(); // IPC_PRIVATE， 在父子间进行通信时,key值不需要显式获取, (匿名IPC)

	semid = semget(IPC_PRIVATE, 1, 0600); // 资源量个数1， IPC_PRIVATE使用时，flags参数需要有值
	if(semid < 0)
	{
		perror("semget()");
		exit(1);
	}

	if(semctl(semid, 0, SETVAL, 1) < 0)  // 设置下标0的元素值为1
	{
		perror("semctl()");
		exit(1);
	}

	for(i=0; i<PROCNUM; i++)
	{
		pid = fork();
		if(pid < 0)
		{
			perror("fork()");
			exit(1);
		}
		if(pid == 0) // 子进程
		{
			func_add();
			exit(0);
		}
	}

	for(i=0; i<PROCNUM; i++)
		wait(NULL);

	semctl(semid, 0, IPC_RMID); // 销毁

	exit(0);
}