#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile int loop = 0;

static void alrm_handler(int s)
{
	loop = 1;
}

static void f1(char *p)
{
	printf("f1(): %s\n", p);

}

static void f2(char *p)
{
	printf("f2(): %s\n", p);
	
}

static void f3(char *p)
{
	printf("f3(): %s\n", p);
	
}

int main(int argc, char **argv)
{
	puts("Begin!");


	// call f1 after 3 secs
	// call f2 after calling f1 2 secs
	// call f3 after calling f2 3 secs

	signal(SIGALRM, alrm_handler);

	alarm(3);

	while(!loop)
	{
		pause();
	}
	loop = 0;

	f1("123");

	alarm(2);

	while(!loop)
	{
		pause();
	}
	loop = 0;


	f2("456");


	puts("End!");

	while(1)
	{
		write(1, '.', 1);
		sleep(1);
	}
		


	exit(0);


}