#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 10

// 从buf位置读取wirte_size大小,写入fp中
static void full_write(char *buf, int total_size, FILE *fp)
{
	int sigle_written, total_written, left;

	total_written = 0;
	left = total_size;

	while(left > 0)
	{
		sigle_written = fwrite(buf + total_written, 1, left, fp);
		total_written += sigle_written;
		left -= sigle_written;
	}
}
int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: ./mycp_1 srcfile desfile");
		exit(1);
	}

	FILE *des = NULL;
	FILE *src = NULL;
	char buf[BUFSIZE];
	int readnum;

	if((src = fopen(argv[1], "r")) == NULL)
	{
		perror("fopen(src)");
		exit(1);
	}


	if((des = fopen(argv[2], "w")) == NULL)
	{
		perror("fopen(des)");
		fclose(src);
		exit(1);
	}


	readnum = fread(buf, 1, BUFSIZE, src);
	while(readnum > 0)
	{
		// have_written = 0;
		// leftnum = readnum
		// while(leftnum > 0)
		// {
		// 	writenum = fwrite(buf + have_written, 1, leftnum, des);
		// 	have_written += writenum;;
		// 	leftnum -= writenum;
			
		// }
		full_write(buf, readnum, des);
		readnum = fread(buf, 1, BUFSIZE, src);
	}

	fclose(src);
	fclose(des);

	exit(0);
}