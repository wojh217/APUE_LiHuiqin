#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	time_t end;
	int64_t count=0;

	end = time(NULL) + 5; // time函数结果为整数

	

	while(time(NULL) <= end) // 每次调用time都会从内核取时间戳，比较耗时
	{
		count++;
	}

	printf("%lld", count);

	exit(0);
}