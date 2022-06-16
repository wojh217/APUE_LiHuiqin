#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>


#define MAXCLIENTS		20
#define MINIDLE			5
#define MAXIDLE			10
#define SIG_NOTIFY		SIG_USR2

enum
{
	STATE_IDLE=0,
	STATE_BUSY
};

struct server_st
{
	int pid;
	int status;
};

struct server_st * serverpool;


static int add_1_server()
{
	get_free_position();

	fork_1_server();
}

static int delete_1_server()
{

}

int main()
{
	// todo 
	// sockopt
	struct sigaction sa, osa;
	int i;
	sigset_t set, oset;
	int idle_cnt=0, busy_cnt=0;


	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD, &sa, &osa);

	sigemptyset(&set);
	sigaddset(&set, SIG_NOTIFY);

	sigprocmask(SIG_BLOCK, &set, &oset);



	serverpool = mmap(NULL, sizeof(struct server_st) * MAXCLIENTS, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if(serverpool == MAP_FAILED)
	{
		perror("mmap()");
		exit(1);
	}

	for(i = 0; i < MAXCLIENTS; i++)
		serverpool[i].pid = -1;


	socket();
	bind();
	listen();

	for(i = 0; i < MAXIDLE; i++)
		add_1_server();


	while(1)
	{
		sigsuspend(&oset);

		scan_pool();

		add_or_delete_server();

		print_server_state();
	}



	sigprocmask(SIG_SETMASK, &oset, NULL);

	exit(0);
}