#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


# define BUFSIZE 1024


int main(int argc, char **argv)
{
	int sfd, dfd=1; // dfd为标准输出
	char buf[BUFSIZE];
	int len, ret, pos;

	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
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

	// do
	// {
	// 	dfd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0660);
	// 	if(dfd < 0)
	// 	{
	// 		if(errno != EINTR)
	// 		{
	// 			close(sfd);
	// 			perror("open()");
	// 			exit(1);
	// 		}
	// 	}
	// }while(dfd < 0);


	while(1)
	{
		len = read(sfd, buf, BUFSIZE);
		if(len < 0)
		{
			if(errno == EINTR)
				continue;
			perror("read()");
			break;
		}
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
				exit(1); //退出所有循环
			}

			pos += ret;
			len -= ret;
		}
	}

	close(sfd);
	// close(dfd); // 不用关闭标准输出

	exit(0);


}