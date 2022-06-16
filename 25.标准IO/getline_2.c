#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void print_buf(char *buf, int bufsize)
{
	int idx;

	for(idx=0; idx < bufsize; idx++)
		printf("%3d ", buf[idx]);
	printf("\n");
}

// getline自己实现且有匹配的free函数
int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		printf("miss filename\n");
		return(-1);
	}

	FILE *fp;
	char *linebuf;
	size_t linesize;
	ssize_t readnum;

	if((fp=fopen(argv[1], "r") ) == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	linebuf = NULL;
	linesize = 0;

	char arr[10];
	linebuf = arr;
	linesize = 10;
	// linesize是申请的内存大小，getline返回读取的字节数
	while (1)
	{
		// 当linebuf为NULL时，将会申请一块新内存，且不会填写linesize的值
		// if (getline(&linebuf, &linesize, fp) < 0 )
		// 	break;
		sleep(1);

		readnum = getline(&linebuf, &linesize, fp);
		if (readnum == -1)
		{
			printf("xxxxx");
			break;
		}
		
		printf("linebuf: %d\n", strlen(linebuf));
		printf("linesize: %d\n", linesize);
		printf("readnum: %d\n", readnum);
	}

	fclose(fp);
	exit(0);
}