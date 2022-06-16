#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 10
#define READSIZE 5

// 打印buf的内容
static void print_buf(char *buf, int bufsize)
{
	int idx;

	for(idx=0; idx < bufsize; idx++)
		printf("%3d ", buf[idx]);
	printf("\n");
}

int main(int argc, char *argv[]) 
{
	if (argc < 2)
	{
		printf("miss filename\n");
		return(-1);
	}

	FILE *fp = NULL;
	char buf[BUFSIZE];
	char *tmp;

	memset(buf, 'A', sizeof(buf));

	print_buf(buf, BUFSIZE);


	fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		perror("fopen()");
		return -1;
	}

	fseek(fp, 0, SEEK_END);

	//读取一次
	tmp = fgets(buf, READSIZE, fp);
	if(tmp == NULL)
	{
		printf("fgets()");
		fclose(fp);
		exit(1);
	}

	print_buf(buf, BUFSIZE);

	printf("tmp:%p", tmp);
	printf("buf:%p", buf);

	fclose(fp);

	

}