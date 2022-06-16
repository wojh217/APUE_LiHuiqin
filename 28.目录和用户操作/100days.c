#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMESTRSIZE 1024
int main()
{

	time_t stamp;
	struct tm *tm;
	char timestr[TIMESTRSIZE];

	stamp = time(NULL);

	tm = localtime(&stamp);

	strftime(timestr, TIMESTRSIZE, "%Y-%m-%d %H:%M:%S", tm);

	printf("Now: %s\n", timestr);

	tm->tm_mday += 100; // 只能加到mday上，不能加到yday上

	mktime(tm);

	strftime(timestr, TIMESTRSIZE, "%Y-%m-%d %H:%M:%S", tm);
	printf("100 days after: %s\n", timestr);

	exit(0);
}