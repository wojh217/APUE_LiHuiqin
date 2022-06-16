#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FILEPATH "/tmp/out"

int main()
{

	int fd, count;
	pid_t pid;

	puts("begin");

	pid = fork();
	if(pid<0)
	{
		perror("fork()");
		exit(1);
	}
	else if(pid == 0)
	{
		printf("[%d]: Child is working!\n", getpid());
		count = 0;
		while(1)
		{
			fd = open(FILEPATH, O_RDONLY);
			if(fd < 0)
			{
				perror("open()");
				break;
			}
			count += 1;
		}

		printf("Child cnt:%d\n", count);

		while(1)
			pause();
		


	}
	else
	{
		printf("[%d]: Parent is working!\n", getpid());

		count = 0;
		while(1)
		{
			fd = open(FILEPATH, O_RDONLY);
			if(fd < 0)
			{
				perror("open()");
				break;
			}
			count += 1;
		}

		printf("Parent cnt:%d\n", count);
		while(1)
			pause();
	}

	// puts("end");
	// close(fd);

}