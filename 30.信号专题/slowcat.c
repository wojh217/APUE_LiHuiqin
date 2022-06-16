#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>


# define CPS 10
# define BUFSIZE CPS

static volatile int loop = 0;

static void alrm_handler(int s)
{
	alarm(1); // 持续发出信号
	loop = 1;
}

int main(int argc, char **argv)
{
	int sfd, dfd=1; // dfd为标准输出
	char buf[BUFSIZE];
	int len, ret, pos;
	int readfail;

	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	signal(SIGALRM, alrm_handler);
	alarm(1);

	do
	{
		sfd = open(argv[1], O_RDONLY);
		if(sfd < 0)
		{
			if(errno != EINTR)
			{
				perror("open()");
				exit(1);
			}
		}
	}while(sfd < 0);


	while(1)
	{
		while(!loop)
			pause();
		loop = 0;

		// 由于read放在while中，因此要增加一个标志位
		readfail = 0;
		while((len = read(sfd, buf, BUFSIZE)) < 0)
		{
			if(errno == EINTR)
				continue;
			perror("read()");
			readfail = 1;
			break;
		};

		if(readfail == 1)
			break;
		
		if(len == 0) //文件结束
			break;

		pos = 0;
		while(len > 0)
		{
			ret = write(dfd, buf + pos, len);
			if(ret < 0)
			{
				if(errno == EINTR)
					continue;
				perror("write()");
				exit(1); //退出所有循环, 未关闭所有打开的文件，有限的内存泄露
			}

			pos += ret;
			len -= ret;
		}
	}

	close(sfd);
	// close(dfd); // 不用关闭标准输出

	exit(0);


}