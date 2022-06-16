#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main()
{
	FILE *fp;
	int count = 0;


	while(1)
	{
		fp = fopen("myfile", "r");
		if(fp == NULL)
		{
			// perror("fopen()");
			// printf("mag is %s\n", strerror(errno));
			fprintf(stderr, "fopen() failed! errno=%d\n", errno);
			fprintf(stderr, "fopen() failed! msg=%s\n", strerror(errno));
			break;
		}
		count++;
	}

	printf("maxfile: %d", count);

	exit(0);
}