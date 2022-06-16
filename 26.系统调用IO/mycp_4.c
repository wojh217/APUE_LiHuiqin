#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 1024

// 在mycp_3中由于栈空间限制大小为8M
// 当BUFSIZE超过8M时就会报段错误
// 这里修改buf不使用栈空间，而使用动态内存分配，看还会报段错误吗？
// 实验查看不会
int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: ./mycp_1 srcfile desfile");
		exit(1);
	}

	int des_fd;
	int src_fd;
	// char buf[BUFSIZE];

	char *buf=NULL;
	int readnum, wrnum, position;

	buf = malloc(BUFSIZE);
	if(buf == NULL)
	{
		perror("malloc()");
		exit(1);
	}

	do
	{
		if((src_fd = open(argv[1], O_RDONLY)) < 0 )
		{
			// 打开的过程中被信号中断，则需要重新打开
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				perror("open(src)");
				exit(1);
			}
			
		}
	}while(src_fd < 0);
	

	do
	{
		if((des_fd = open(argv[2], O_WRONLY|O_TRUNC|O_CREAT, 0664)) < 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				perror("open(des)");
				close(src_fd);
				exit(1);
			}
			
		}
	}while(des_fd < 0);
	


	
	while(1)
	{
		readnum = read(src_fd, buf, BUFSIZE);
		if (readnum < 0)
		{
			// 还未读取到任意数据之前被信号中断
			// the call was interrupted by a signal before any data wsa read
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				perror("read()");
				break;
			}
			
		}
			
		if (readnum == 0) // 读到文件尾
			break;

		position = 0;
		while(readnum > 0)
		{
			wrnum = write(des_fd, buf + position, readnum);
			if (wrnum < 0)
			{	
				if(errno == EINTR)
				{
					continue;
				}
				else
				{
					perror("write()");
					exit(1); // 允许小的内存泄露
				}
				
			}
			readnum -= wrnum;
			position += wrnum;
		}
	}

	close(src_fd);
	close(des_fd);

	free(buf);
	buf = NULL;

	exit(0);
}