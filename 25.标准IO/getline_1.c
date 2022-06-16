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

static int mygetline(char **p, int *bufsize, FILE *fp)
{

	int sub_flag  = 0;
	int find_flag = 0;
	int ii = 0;
	int oldsize = 0;
	int newsize;
	

	if (bufsize == NULL)
	{
		return -1;
	}

	if (*p == NULL)
	{
		if ((*p = malloc(BEGINSIZE)) == NULL)
		{
			perror("malloc()");
			return -1;
		};
		newsize = BEGINSIZE;
	}

	else
	{
		if(*bufsize > 0)
		{
			newsize = *bufsize;
		}
		else
		{
			newsize = BEGINSIZE;
		}
		
	}



	while (find_flag == 0)
	{
		memset(*p + oldsize - sub_flag, 0, newsize - oldsize + sub_flag);
		fgets(*p + oldsize - sub_flag, newsize - oldsize + sub_flag, fp);

		for(ii=oldsize - sub_flag; ii<newsize-1; ii++)
		{
			if(buf[ii] == '\n' || buf[ii] == '\0')
			{
				find_flag = 1;
				break;
			}
		}


		if (find_flag == 0)
		{
			oldsize = newsize;
			newsize = newsize * 2;

			if ((*p = realloc(*p, newsize)) == NULL);
			{
				perror("realloc()");
				return -1;
			}
		}

		sub_flag = 1;
	}

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
	char *buf;
	char *tmp;
	int ii;
	int find_flag;
	int newsize, oldsize;
	

	if((fp=fopen(argv[1], "r") ) == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	// malloc申请的内存,会自动清0吗?(不会)
	buf = NULL;
	oldsize = 0;
	newsize = BEGINSIZE;
	find_flag = 0;
	int readnum;
	int sub_flag  = 0;

	while (find_flag == 0)
	{

		tmp = realloc(buf, newsize);
		if (tmp == NULL)
		{
			perror("realloc()");
			fclose(fp);
			exit(1);
		}

		buf = tmp;
		// 新申请的内存清零
		memset(buf + oldsize - sub_flag, 0, newsize - oldsize + sub_flag);

		// 读取内容
		fgets(buf + oldsize - sub_flag, newsize - oldsize + sub_flag, fp);

		// // 读取内容
		// readnum = fread(buf + oldsize, 1, newsize - oldsize, fp);

		// // 读取到0块，可能发生错误，也可能文件结束
		// if (readnum == 0)
		// {
		// 	// 文件结束
		// 	if(feof(fp) != 0 )
		// 	{
		// 		// buf里存了完整的一行数据，且没有尾0
		// 	}
		// 	else
		// 	{
		// 		perror("fread()");
		// 		fclose(fp);
		// 		exit(1);
		// 	}
		// }
			


		// 如何判断读取到了换行标志符(\n)?
		// buf未填满,则肯定读取到了换行;
		// buf填满且最后读到的刚刚好是\n
		// 循环判断buf的每一位,判断是否有\n

		for(ii=oldsize - sub_flag; ii<newsize-1; ii++)
		{
			if(buf[ii] == '\n' || buf[ii] == '\0')
			{
				find_flag = 1;
				buf[ii] = '\0';
				break;
			}
		}


		if (find_flag == 0)
		{
			oldsize = newsize;
			newsize = newsize * 2;
		}

		sub_flag = 1;
		
	}


	fclose(fp);

	printf("ii=%d\n", ii);
	printf("oldsize=%d\n", oldsize);
	printf("newsize=%d\n", newsize);

	print_buf(buf, newsize);
	puts(buf);


	exit(0);
}