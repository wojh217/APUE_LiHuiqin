#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void mytest(char **p)
{
	// char *buf = *p;

	*p = malloc(4);


}


static void mytest2(int *p)
{
	// char *buf = *p;

	int *m;
	m = p;
	*m = 4;


}


int main()
{

	char *ch = NULL;
	printf("ch->: %p\n", ch);

	mytest(&ch);

	printf("ch->: %p\n", ch);



	exit(0);

}