#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROCNUM  20
#define LINESIZE 1024
#define FILENAME "/tmp/out"

//多进程打开文件，(使用文件锁)取数加一重新写入

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

	fd = fileno(fp);

	lockf(fd, F_LOCK, 0);
	fgets(linebuf, LINESIZE, fp);
	fseek(fp, 0, SEEK_SET);
	sleep(1);
	fprintf(fp, "%d\n", atoi(linebuf) +1);
	fflush(fp);
	lockf(fd, F_ULOCK, 0); // 释放文件锁

	fclose(fp);
	return ;
}

int main()
{

	int i;
	pid_t pid;

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

	exit(0);
}