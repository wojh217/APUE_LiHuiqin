#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
	while(1)
	{
		prompt(); // 打印提示符

		getline(); // 捕获输入行

		parse(); // 解析命令

		// 分内部和外部命令
		if()  // 内部命令
		{

		}
		else()
		{

		}


	}
	exit(0);
}