#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static void * thr_func(void *p)
{

	sleep(100);
	pthread_exit(NULL);
}

int main()
{
	pthread_t tid;
	pthread_attr_t attr;
	int err;

	pthread_attr_init(&attr);
	// pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// detached thread
	err = pthread_create(&tid, &attr, thr_func, NULL);
	if(err != 0 )
	{
		fprintf(stderr, "pthread_create():%s\n", strerror(err));
		exit(1);
	}
	
	sleep(5);




	// pthread_attr_destroy(&attr);
	pthread_exit(NULL);
	// exit();
}