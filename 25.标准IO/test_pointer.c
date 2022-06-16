#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static int mytest(char **p)
{
	if(*p == NULL)
	{
		if ((*p = malloc(3)) == NULL)
		{
			// perror("malloc()");
			printf("malloc error");
			return -1;
		};

		printf("*p1: %p\n", *p);

		if ((*p = realloc(*p, 6)) == NULL)
		{
			// perror("realloc()");
			printf("realloc error");
			return -2;
		};

		printf("*p2: %p\n", *p);
		return 0;
	}

}
int main()
{

	char *ch = NULL;
	// char **p;

	// p = &ch;

	// printf("ch-> %p\n", ch);

	// *p = malloc(4);

	// printf("ch-> %p\n", ch);

	// if((*p = realloc(*p, 8)) == NULL)
	// {
	// 	printf("realloc error");
	// 	exit(1);
	// }	

	// printf("ch-> %p\n", ch);

	mytest(&ch);

	printf("ch: %p\n", ch);

	exit(0);

}