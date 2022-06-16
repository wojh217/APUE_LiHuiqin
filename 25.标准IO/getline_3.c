#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BEGINSIZE 4

static void print_buf(char *buf, int bufsize)
{
	int idx;

	for(idx=0; idx < bufsize; idx++)
		printf("%3d ", buf[idx]);
	printf("\n");
}

// 不足的时候才更改bufsize值
static int mygetline(char **p, int *bufsize, FILE *fp)
{

	int sub_flag  = 0;
	int find_flag = 0;

	int ii = 0;
	int oldsize = 0;
	int newsize;
	char *tmp = NULL;
	int get_flag = 1;
	

	if (bufsize == NULL)
		return -1;

	if (*p == NULL)
	{
		printf("*p == NULL\n");
		if ((*p = malloc(BEGINSIZE)) == NULL)
		{
			perror("malloc()");
			return -1;
		};
		newsize = BEGINSIZE;
	}

	else
	{
		printf("*p != NULL\n");
		printf("*bufsize = %d\n", *bufsize);
		if(*bufsize > 0)
		{
			newsize = *bufsize;
		}
		else
		{
			newsize = BEGINSIZE;
		}


		
	}

	printf("*oldsize = %d\n", oldsize);
	printf("*newsize = %d\n", newsize);


	while (find_flag == 0)
	{
		printf("aaa\n");

		memset(*p + oldsize - sub_flag, 0, newsize - oldsize + sub_flag);

		print_buf(*p, newsize);


		if (fgets(*p + oldsize - sub_flag, newsize - oldsize + sub_flag, fp) == NULL)
		{
			return -1;
		}	

		print_buf(*p, newsize);

		for(ii=oldsize - sub_flag; ii<newsize-1; ii++)
		{
			if((*p)[ii] == '\n' || (*p)[ii] == '\0')
			{
				if(ii == oldsize - sub_flag)
				{

				}
				find_flag = 1;
				break;
			}
		}

		

		if (find_flag == 0)
		{
			printf("bbb\n");

			oldsize = newsize;
			newsize = newsize * 2;

			printf("*oldsize = %d\n", oldsize);
			printf("*newsize = %d\n", newsize);

			if ((*p = realloc(*p, newsize)) == NULL)
			{
				printf("realloc() error\n");
				perror("realloc");
				return -1;
			}
		}

		sub_flag = 1;
	}

	if (*bufsize < newsize)
	{
		*bufsize = newsize;
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

	char arr[10];
	linebuf = arr;
	linesize = 10;

	while (1)
	{
		sleep(1);
		if (mygetline(&linebuf, &linesize, fp) < 0 )
		{
			printf("mygetline return < 0\n");
			break;
		}
		
		printf("linebuf: %d\n", strlen(linebuf));
		printf("linesize: %d\n", linesize);
	}

	fclose(fp);
	exit(0);


}