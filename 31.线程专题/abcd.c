#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THRNUM 4



// 使用四个互斥量的锁链，来实现按顺序打印abcd
// (忙等版)
static pthread_mutex_t mux[THRNUM];

static int next(int n)
{
	if(n + 1 == THRNUM)
		return 0;
	return n+1;
}

static void * func(void *p)
{
	int c='a' + (int)p;
	int n = (int)p;

	while(1)
	{
		pthread_mutex_lock(mux + n);
		write(1, &c, 1);
		pthread_mutex_unlock(mux + next(n));
	}

	pthread_exit(NULL);
}

int main()
{
	pthread_t tid[THRNUM];

	int i;
	for (i=0; i < THRNUM; i++)
	{
		pthread_mutex_init(mux+i, NULL);
		pthread_mutex_lock(mux+i);

		pthread_create(tid+i, NULL, func, (void *)i);
	}

	alarm(5);
	pthread_mutex_unlock(mux);

	for (i=0; i < THRNUM; i++)
	{
		pthread_join(tid[i], NULL);
	}


	exit(0);

}