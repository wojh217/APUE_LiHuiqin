#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

static void * func(void *arg)
{
	puts("Thread is working!");
	// return NULL;
	pthread_exit(NULL);  // 
}


int main()
{
	pthread_t tid;
	int err;

	puts("Begin!");

	err = pthread_create(&tid, NULL, func, NULL);
	if(err)
	{
		fprintf(stderr, "pthread_create(): %s\n",strerror(err));
		exit(1);
	}

	pthread_join(tid, NULL);

	puts("End!");

	exit(0);


}