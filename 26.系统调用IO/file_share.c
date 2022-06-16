#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>


// 删除一个文件的第二行
// 利用文件共享，使用多线程对一个文件打开多次
int main(int argc, char **argv)
{

	if(argc < 2)
	{
		fprintf(stdout, "miss filename")；
		exit(0);
	}

	int fd1, fd2;

	// 一个线程打开文件用来读
	fd1 = open(argv[1], O_RDONLY);
}