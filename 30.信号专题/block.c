#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void sig_handler(int s)
{
	write(1, "!", 1);
}

int main()
{
	int i,j;
	sigset_t set, oset, saveset;


	signal(SIGINT, sig_handler);

	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	// 保存初始状态
	sigprocmask(SIG_UNBLOCK, &set, &saveset);
	for(j = 0; j < 1000; j++)
	{
		// 将set中信号增加到block集
		sigprocmask(SIG_BLOCK, &set, NULL); // 该信号的maks位置为0

		/* 或者存储之前状态，其后再恢复
		sigprocmask(SIG_BLOCK, &set, &oset); 
		临界区
		sigprocmask(SIG_SETMASK, &oset, NULL);

		*/
		for(i=0; i<5; i++)
		{
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);

		sigprocmask(SIG_UNBLOCK, &set, NULL);
	}

	// 确保进入和离开模块后，信号的行为不发生改变
	sigprocmask(SIG_SETMASK, &saveset, NULL);
}