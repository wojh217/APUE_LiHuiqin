#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int mytest(char **p)
{

	if(*p == NULL)
	{
		if ((*p = malloc(3)) == NULL)
		{
			perror("malloc()");
			return -1;
		};

		printf("*p1: %p\n", *p);

		if ( (*p = realloc(*p, 6)) == NULL)
		{
			printf("realloc() error\n");
			perror("realloc");
			return -2;
		}

		printf("*p2: %p\n", *p);
		return 0;

	}

	
}

// 动态申请内存,用来存储从文件中读取一行数据
int main(int argc, char ** argv)
{

	char *linebuf=NULL;
	// linebuf = NULL;

	mytest(&linebuf);

	printf("linebuf: %p\n", linebuf);
	

	exit(0);


}