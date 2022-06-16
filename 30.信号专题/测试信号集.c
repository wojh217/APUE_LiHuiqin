#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


static void handler_1(int sig)
{
	printf("receive sig: %d\n", sig);
}

int main()
{
	sigset_t set, oset, wait;
	struct sigaction  sa, osa;
	struct sigaction usrsa, ousrsa;
	int i;

	sa.sa_handler = handler_1;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);

	usrsa.sa_handler = SIG_DFL;
	sigemptyset(&usrsa.sa_mask);
	usrsa.sa_flags = 0;
	sigaction(SIGUSR2, &usrsa, NULL);


	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	sigemptyset(&wait);
	sigaddset(&wait, SIGUSR1);

	sigprocmask(SIG_BLOCK, &set, &oset);

	// sigprocmask(SIG_SETMASK, &set, NULL);

	// 不希望被信号打断的代码
	for(i=0; i<20; i++)
	{
		printf("i:%d\n", i);
		sleep(1);
	}

	sigsuspend(&oset);

	printf("sig event\n");
		


	exit(0);

}