#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static void clean(void *p)
{
	puts(p);
}

static void * func(void *p)
{

	pthread_cleanup_push(clean, "clean_1");
	pthread_cleanup_push(clean, "clean_2");


	sleep(10);

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(0);

	

	pthread_exit(NULL);

}

int main()
{
	pthread_t tid;


	pthread_create(&tid, NULL, func, NULL);


	pthread_cancel(tid);
	pthread_join(tid, NULL); 



	puts("join end");


	exit(0);
}