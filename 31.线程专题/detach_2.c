#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static void * thr_func(void *p)
{
	sleep(5);
	printf("111\n");

	pthread_exit(NULL);
}

int main()
{
	pthread_t tid;
	pthread_attr_t attr;
	int err, err2;

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

	err2 = pthread_join(tid, NULL);
	if(err2 != 0)
	{
		fprintf(stderr, "pthread_join():%s\n", strerror(err));
		exit(1);
	}

	printf("2222\n");

	sleep(100);

	// pthread_attr_destroy(&attr);
	// pthread_exit(NULL);
	exit(0);
}