#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FNAME "/tmp/out"
#define BUFFSIZE 1024

// 为什么没有写入成功，因为不是行缓冲模式吗？
int main()
{

	FILE * fp;
	char buf[BUFFSIZE];
	int count = 0;
	time_t stamp;
	struct tm *mytm;

	fp = fopen(FNAME, "a+"); // a+ 读从文件头 写从文件尾
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}


	// 获取行数 //缓冲区设计大一些，保证每次完整读取一行
	while(fgets(buf, BUFFSIZE, fp) != NULL)
	{
		count++;
	}

	// 将时间写入文件
	// 如果每次sleep(1)后，将当前时间写入文件，那么写入操作如果是阻塞操作，也将会占用时间
	// 则下次准备写入文件时，经过的时间已经超过一秒

	while(1)
	{
		time(&stamp);
		mytm = localtime(&stamp);
		// strftime();
		fprintf(fp, "%-4d%d-%d-%d %d:%d:%d\n", ++count, \
			mytm->tm_year+1900, mytm->tm_mon+1, mytm->tm_mday,\
			 mytm->tm_hour, mytm->tm_min, mytm->tm_sec); // %-4d左对齐

		fflush(fp); // 手动刷新缓冲区

		
		sleep(1);
	}

	fclose(fp);

	exit(0);
}