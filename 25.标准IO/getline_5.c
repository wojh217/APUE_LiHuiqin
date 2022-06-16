#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLOCKSIZE 4

static void print_buf(char *buf, int bufsize)
{
	int idx;

	for(idx=0; idx < bufsize; idx++)
		printf("%3d ", buf[idx]);
	printf("\n");
}

// 不足的时候才更改bufsize值
// 修改getline3中传指针不为空，扩充内存时出错的问题(realloc的*p必须是由malloc或alloc申请的地址)
// *p要么为malloc申请的指针地址，要么为NULL
static int mygetline(char **p, int *bufsize, FILE *fp)
{

	int ii = 0;
	int find_flag = 0;

	int begin = 0;
	int end;
	long position;
	
	printf("---------------\n");
	if (bufsize == NULL)
		return -1;
	

	// *p为空或*p的长度小于2, 申请内存 || *bufsize < 2
	if (*p == NULL)
	{
		printf("*p == NULL\n");

		if ((*p = malloc(BLOCKSIZE)) == NULL)
		{
			perror("malloc()");
			return -1;
		};

		end = BLOCKSIZE;
	}
	else
	{
		if (*bufsize < 1)
			return -1;
		end = *bufsize;
	}

	if (end < 2)
	{
		end = end * 2;
		if ((*p = realloc(*p, end)) == NULL)
			{
				printf("realloc() error\n");
				perror("realloc");
				return -1;
			}
	}

	
	while (find_flag == 0)
	{
		printf("aaa\n");

		memset(*p + begin, 0, end - begin);

		print_buf(*p, end);


		if (fgets(*p + begin, end - begin, fp) == NULL)
		{
			return -1;
		}	

		print_buf(*p, end);

		for(ii= begin; ii < end - 1; ii++)
		{
			if((*p)[ii] == '\n' || (*p)[ii] == '\0')
			{
				find_flag = 1;
				break;
			}
		}

		if (find_flag == 0)
		{
			printf("bbb\n");

			begin = end - 1;
			end  = end * 2;


			if ((*p = realloc(*p, end)) == NULL)
			{
				printf("realloc() error\n");
				perror("realloc");
				return -1;
			}
		}

	}

	if (*bufsize < end)
	{
		*bufsize = end;
	}


	if((*p)[ii] == '\n')
		return ii+1;
	else
		return ii;
}

// 动态申请内存,用来存储从文件中读取一行数据
int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		printf("miss filename\n");
		return(-1);
	}

	FILE *fp;
	char *linebuf;
	int linesize;

	if((fp=fopen(argv[1], "r") ) == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	linebuf = NULL;
	linesize = 0;

	linebuf = malloc(10);
	linesize = 10;

	while (1)
	{
		sleep(1);
		if (mygetline(&linebuf, &linesize, fp) < 0 )
		{
			break;
		}
		
		printf("linebuf: %d\n", strlen(linebuf));
		printf("linesize: %d\n", linesize);
	}

	fclose(fp);
	exit(0);


}