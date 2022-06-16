#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mypipi.h"

struct mypipe_st
{
	int head;
	int tail;
	char data[PIPISIZE];
	int datasize; 			// 表示当前管道数据个数
	pthread_mutex_t mut;	// 对管道的读和写都应该是阻塞的，那么有通知法和轮询法
	pthread_cond_t cond;	// 通知法使用条件变量
};


//
mypipe_t *mypite_init(void)
{
	struct mypipe_st *me;
	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	me->head = 0;
	me->tail = 0;
	me->datasize = 0;
	pthread_mutex_init(&me->mut, NULL);
	pthread_cond_init(&me->cond, NULL);

	return me;

}

static int mypite_readbyte_unlock(struct mypipe_st *me, char *datap)
{
	if(me->datasize <= 0)
		return -1;

	*datap = me->data[me->head];

	me->head = next(me->head);
	me->datasize --;

	return 0;
}

//仿照系统io--read
int mypite_read(mypipe_t *ptr, void *buf, size_t count)
{
	struct mypipe_st *me = ptr;
	size_t i;

	pthread_mutex_lock(&me->mut);

	// 阻塞读
	while(me->datasize <= 0)
		pthread_cond_wait(&me->cond, &me->mut); // 解锁等待，等待管道中有数据

	for(i = 0; i < count; i++)
	{
		if(mypite_readbyte_unlock(me, buf+i) != 0)
			break;
	}

	pthread_cond_broadcast(&me->cond); // 惊群通知
	pthread_mutex_unlock(&me->mut);

	return i;
}


int mypite_write(mypipe_t *ptr, const void *buf, size_t count)
{
	struct mypipe_st *me = ptr;

	pthread_mutex_lock(&me->mut);



	pthread_mutex_unlock(&me->mut);

	return n;
}

int mypite_destroy(mypipe_t *ptr)
{
	struct mypipe_st *me = ptr;
	free(me);
	pthread_mutex_destroy(&me->mut);
	pthread_cond_destroy(&me->cond);

	return 0;
}