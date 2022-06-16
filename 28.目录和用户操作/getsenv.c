#include <stdio.h>
#include <stdlib.h>


// 注意extern的使用
extern char **environ;
int main()
{

	int i;
	for(i=0; environ[i] != NULL; i++)
		puts(environ[i]);

	printf("-----------------\n");
	puts(getenv("PATH"));

	exit(0);
}