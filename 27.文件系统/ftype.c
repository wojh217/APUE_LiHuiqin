#include <stdio.h>
#include <stdlib.h>

// char 类型有没有符号不确定，因此最好用int代替
static int ftype(const char* fname)
{
	struct stat statres;
	 // 传递结构体变量的地址
	if(stat(fname, &statres) < 0)
	{
		perror("stat()");
		exit(1);
	}

	if(S_ISREG(statres.st_mode) )
		return '';
	else if()
		return '';
	else 
		return '';

}
int main(int argc, char **argv)
{

	int fd;
	if(argc < 2)
	{
		fprintf(stderr, "Usage..\n");
		exit(1);
	}

	exit(0);
}