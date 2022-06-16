#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 10


int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: ./mycp_1 srcfile desfile");
		exit(1);
	}

	int des_fd;
	int src_fd;
	char buf[BUFSIZE];
	int readnum, wrnum, position;

	if((src_fd = open(argv[1], O_RDONLY)) < 0 )
	{
		perror("open(src)");
		exit(1);
	}

	// 带有O_CREAT的需要有mode权限
	// 如果不带则会出现这样
	// ---Sr-s--T. 1 root root 35278926 3月  26 20:54 /tmp/out
	if((des_fd = open(argv[2], O_WRONLY|O_TRUNC|O_CREAT, 0664)) < 0)
	{
		perror("open(des)");
		close(src_fd);
		exit(1);
	}


	
	while(1)
	{
		readnum = read(src_fd, buf, BUFSIZE);
		if (readnum < 0)
		{
			perror("read()");
			break;
		}
			
		if (readnum == 0) // 真实情况下有可能会被信号打断
			break;

		position = 0;
		while(readnum > 0)
		{
			wrnum = write(des_fd, buf + position, readnum);
			if (wrnum < 0)
			{
				perror("write()");
				exit(1); // 允许小的内存泄露
			}
			readnum -= wrnum;
			position += wrnum;
		}
	}

	close(src_fd);
	close(des_fd);

	exit(0);
}