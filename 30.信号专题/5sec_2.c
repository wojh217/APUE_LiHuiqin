#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

static volatile int loop = 1;

static void alrm_handler(int s)
{
	loop = 0;
}

static void sig_handler(int s, siginfo_t *infop, void *p)
{

	loop = 0;
	printf("111\n");
}


int main()
{
	int64_t count=0;
	struct sigaction act, oact;
	struct itimerval itv, oitv;

	//重新注册alarm信号的信号处理函数，防止杀死进程
	// signal(SIGALRM, alrm_handler); 

	// 使用sigaction代替signal, 并将之前的信号行为保存
	act.sa_sigaction = sig_handler;
	sigemptyset(&act.sa_mask);  // 信号处理函数执行中是否屏蔽其他信号
	act.sa_flags = SA_SIGINFO; // 表示是三参的处理函数

	sigaction(SIGALRM, &act, &oact);

	// 用setitimer代替
	// alarm(5);

	itv.it_interval.tv_sec = 0; //周期信号
	itv.it_interval.tv_usec = 0;

	itv.it_value.tv_sec = 5;  //下一次信号
	itv.it_value.tv_usec = 0;

	setitimer(ITIMER_REAL, &itv, &oitv);



	// 循环体内不涉及到while中的判断条件，程序优化时会将loop置为永真
	// 需要将loop用volatile修饰，才可进行优化
	while(loop)
	{
		count++;
	}

	printf("%lld", count);

	exit(0);
}