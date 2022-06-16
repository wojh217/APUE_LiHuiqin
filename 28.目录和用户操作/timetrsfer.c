#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STRMAX 128
int main()
{
	time_t stamp;
	struct tm *mytm;
	char ch[STRMAX];

	stamp = time(NULL); // 或者 time(&stamp)

	printf("current timestamp: %lld\n", (long long)stamp);


	mytm = localtime(&stamp);

	printf("current Year: %d, Month: %d, Day: %d\n", mytm->tm_year+1900, mytm->tm_mon+1, mytm->tm_mday);
	printf("current hours: %d, minutes: %d, seconds: %d\n", mytm->tm_hour, mytm->tm_min, mytm->tm_sec);


	strftime(ch, STRMAX, "%Y-%m-%d %H:%M:%S", mytm);

	printf("current timestr: %s\n", ch);

	exit(0);
}