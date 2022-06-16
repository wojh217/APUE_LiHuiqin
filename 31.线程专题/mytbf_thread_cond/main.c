#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "mytbf.h"

# define CPS 10
# define BUFSIZE 1024
# define BURST 100 // token最大值


int main(int argc, char **argv)
{
	int sfd, dfd=1; // 1为标准输出
	char buf[BUFSIZE];
	int len, ret, pos;
	int readfail;
	mytbf_t *tbf;
	int size;

	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	tbf = mytbf_init(CPS, BURST);
	if(tbf == NULL)
	{
		fprintf(stderr, "mytbf_init() error");
		exit(1);
	}


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
		// mytbf_fetchtoken内部实现pause阻塞
		size = mytbf_fetchtoken(tbf, BUFSIZE); // 一次要消耗的token数，最大值不超过BUFSIZE
		if(size < 0)
		{
			fprintf(stderr, "mytbf_fetchtoken():%s\n", strerror(-size));
			exit(1);
		}

		// 由于read放在while中，因此要增加一个标志位
		readfail = 0;
		while((len = read(sfd, buf, size)) < 0)
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

		// 一次要消耗的token数，代表每次要读取的字节数
		// 如果真实读取的字节数小于该值，则需要将差值再还给token
		if(size - len > 0)
			mytbf_returntoken(tbf, size - len);

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
	//
	mytbf_destroy(tbf);

	exit(0);


}