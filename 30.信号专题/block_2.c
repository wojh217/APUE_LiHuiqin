#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>


static void act_handler(int s, siginfo_t *infop, void *p)
{
	write(1, "!", 1);
}

static void sig_handler(int s)
{
	write(1, "!", 1);
}


int main()
{
	struct sigaction act, oact;
	struct itimerval itv, oitv;
	int i,j;
	sigset_t set, oset, saveset;

	// 信号处理函数
	act.sa_sigaction = act_handler;
	sigemptyset(&act.sa_mask);  // 信号处理函数执行中是否屏蔽其他信号
	act.sa_flags = SA_SIGINFO; // 表示是三参的处理函数
	sigaction(SIGINT, &act, &oact);


	// signal(SIGINT, sig_handler);

	// // 发出信号
	// itv.it_interval.tv_sec = 0; //周期信号
	// itv.it_interval.tv_usec = 0;
	// itv.it_value.tv_sec = 5;  //下一次信号
	// itv.it_value.tv_usec = 0;
	// setitimer(ITIMER_REAL, &itv, &oitv);

	// 获取默认的信号集(包含信号和信号的默认处理行为)

	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	// 这个地方存储之前的
	sigprocmask(SIG_UNBLOCK, &set, &saveset);
	
	for(i = 0; i < 1000; i++)
	{	// 屏蔽信号集中的信号
		sigprocmask(SIG_BLOCK, &set, NULL);
		for(j = 0; j < 5; j++)
		{
			write(1, "*", 1);
			usleep(1000000);
		}
		write(1, "\n", 1);
		// pause();

		// 恢复信号集中的信号
		sigprocmask(SIG_UNBLOCK, &set, NULL);
	} 

	sigprocmask(SIG_SETMASK, &saveset, NULL);

	exit(0);
}