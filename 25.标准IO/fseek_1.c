#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: ./fseek_1 filepath");
		exit(1);
	}

	//通过seek计算文件大小

	FILE *fp;
	long filesize;

	fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);

	fclose(fp);

	printf("file size: %ld", filesize); //long类型打印用ld

	exit(0);

}