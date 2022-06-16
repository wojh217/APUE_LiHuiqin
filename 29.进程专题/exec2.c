#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// fork exec wait的结合使用
// 在终端上输入ls, 相当于fork出一个子进程，子进程执行exec("ls"),然后shell这个父进程wait此子进程
// 因此如果此子进程为单进程模式，一定是子进程执行结束后父进程才返回(即终端显示命令提示符)
// 如果子进程是多进程，且子进程没有wait它自身创建的孙进程就返回，则终端提示符先打印，而孙进程的打印在其后
//    那为什么shell上运行的程序，不管自身还是自身又创建的子进程，都会将打印显示在此shell终端？
//    因为fork时将文件描述符也复制了
int main()
{
	pid_t pid;

	puts("begin...");

	fflush(NULL);

	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}
	else if(pid == 0)
	{
		execl("/bin/date", "date", "+%s", NULL); // 子进程用另外的命令替换 这里"data"参数是显示在ps axf中的名称
		perror("exec()");
		exit(1);
	}

	wait(NULL);
	
	

	puts("end...");
	exit(0);
}