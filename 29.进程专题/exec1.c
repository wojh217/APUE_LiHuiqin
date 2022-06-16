#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 将此文件重定向到/tmp/out，发现没有打印begin
// 是因为后来的进程连标准输入输出都给替换掉了？
// 所以在exec前也要执行fflush刷新缓冲区
int main()
{

	puts("begin...\n");

	fflush(NULL);
	
	execl("/bin/date", "date", "+%s", NULL); // 如果成功替换，则函数不返回，函数后的所有语句都不执行
	perror("exec()");
	

	puts("end...\n");
	exit(0);
}
