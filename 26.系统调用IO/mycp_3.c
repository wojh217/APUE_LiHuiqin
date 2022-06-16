#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 1024

// 修改mycp_2中文件打开、读、写时由于信号打断引起的错误
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

	exit(0);
}