#include <stdlib.h>
#include <stdio.h>

int main()
{
	char ch[100];
	int day=1;
	int month = 2;
	int year = 1992;

	sprintf(ch, "%d-%d-%d", year, month, day);

	puts(ch);

	exit(0);	
}