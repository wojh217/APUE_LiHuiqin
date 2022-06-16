#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) 
{
	if (argc < 2)
	{
		printf("miss filename\n");
	}

	FILE *fp = NULL;

	fp = fopen(argv[1], "w");
	if (fp == NULL)
	{
		perror("fopen()");
		return -1;
	}

	// fseek(fp, 10, SEEK_SET);


	fseek(fp, 10, SEEK_SET);

	fwrite("123456", 1, 3, fp);
	fclose(fp);


}