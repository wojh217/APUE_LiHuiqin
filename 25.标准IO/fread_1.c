#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 10
#define READSIZE 5
#define BLOCKNUM 5
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

	memset(buf, 'A', sizeof(buf));

	print_buf(buf, BUFSIZE);

	fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		perror("fopen()");
		return -1;
	}

	//读取一次
	// fgets(buf, READSIZE, fp);
	fread(buf, 1, BLOCKNUM, fp);

	print_buf(buf, BUFSIZE);

	fclose(fp);

	FILE *fp2 = NULL;
	fp2 = fopen("dest.txt", "w");
	// fputs(buf, fp2);
	fwrite(buf, 1, BLOCKNUM, fp2);
	fclose(fp2);

	exit(0);

}