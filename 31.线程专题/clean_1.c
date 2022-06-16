#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>


static void cleanup_func(void *arg)
{
	puts(arg);
}
static void * func(void *arg)
{
	puts("Thread is working!");
	

	pthread_cleanup_push(cleanup_func, "clean1");
	pthread_cleanup_push(cleanup_func, "clean2");
	pthread_cleanup_push(cleanup_func, "clean3");

	puts("Push over!");

	

	sleep(5);

	pthread_exit(NULL);  // 放在pthread_exit后面的pop不管参数如何，都会执行清理函数

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(1);
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