#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
	pid_t pid;
	int i;
	struct sigaction sa, osa;

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD, &sa, &osa);


	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}
	if(pid == 0)
	{

		for(i=0; i<10; i++)
			sleep(1);
		exit(0);
	}
	else
	{
		for(i=0; i<20; i++)
		{
			printf("child pid: %d, exists: %d\n", pid, kill(pid, 0));
			sleep(1);
		}
	}
	// wait(NULL);

	exit(0);
}