#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include "mytbf.h"
#include <pthread.h>


static struct mytbf_st* job[MYTBF_MAX]; //存放不同实例的数组
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;

static pthread_t tid_alrm;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

struct mytbf_st
{
	int cps; //每秒要处理(即read)的字节数
	int burst; 	// 令牌总数的最大值，其令牌数不能超过该最大值
	int token; // 令牌总数，即收到信号+cps,处理一次减去cps
	int pos;  // 该任务组在数组中的下标位置
	pthread_mutex_t mut;
	pthread_cond_t cond;
};


static void * thr_alrm(void *p)
{
	int i;
	// alarm(1);

	//每个组元素的token增加
	while(1)
	{
		pthread_mutex_lock(&mut_job);

		for(i = 0; i < MYTBF_MAX; i++)
		{
			if(job[i] != NULL)
			{
				pthread_mutex_lock(&job[i]->mut);
				job[i]->token += job[i]->cps;
				if(job[i]->token  > job[i]->burst)
					job[i]->token  = job[i]->burst;

				pthread_cond_broadcast(&job[i]->cond); //给所有等待此条件的发信号 pthread_cond_signal函数只能发一个
				pthread_mutex_unlock(&job[i]->mut);
			}
		}
		pthread_mutex_unlock(&mut_job);
		sleep(1);
	}
}

static void module_unload(void)
{
	int i;

	// 先取消，后join
	pthread_cancel(tid_alrm);
	pthread_join(tid_alrm, NULL);

	for(i = 0; i < MYTBF_MAX; i++)
	{
		if(job[i] != NULL)
		{
			mytbf_destroy(job[i]);
		}

	}

	pthread_mutex_destroy(&mut_job);
}

static void module_load(void )
{
	// alrm_handler_save = signal(SIGALRM, alrm_handler); // alrm_handler_save保存原来的信号处理行为
	// alarm(1);


	int err;

	err = pthread_create(&tid_alrm, NULL, thr_alrm, NULL);
	if(err)
	{
		fprintf(stderr, "pthread_create(): %s\n", strerror(err));
		exit(1);
	}

	atexit(module_unload); //钩子函数，这里时候时候会执行钩子函数？
}



static int get_free_pos_unlocked(void)
{
	int i;

	for(i = 0; i< MYTBF_MAX; i++)
	{
		if(job[i] == NULL)
			return i;
	}
	return -1;
}
mytbf_t * mytbf_init(int cps, int burst)
{
	struct mytbf_st *me;
	int pos;


	

	pthread_once(&init_once, module_load);


	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	pthread_mutex_init(&me->mut); //初始化其token锁
	pthread_cond_init(&me->cond);

	// 临界区，临界区优化，越少越号
	pthread_mutex_lock(&mut_job, NULL);
	pos = get_free_pos_unlocked(); // 函数内部不再需要加锁，但要防止有人未加锁就调用函数，因此函数名后缀unlocked
	if(pos < 0)
	{
		pthread_mutex_unlock(&mut_job);
		free(me);
		return NULL;
	}

	
	me->pos = pos;

	job[pos] = me;
	pthread_mutex_unlock(&mut_job);

	return me;
}

static int min(int a, int b)
{
	if(a<b)
		return a;
	return b;
}


int mytbf_fetchtoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;
	int n;

	if(size <= 0)
		return -EINVAL;
	
	pthread_mutex_lock(&me->mut);
	while(me->token <= 0)
	{
		pthread_cond_wait(&me->cond, &me->mut); // 解锁等待（临界区外等待），等signal打断，打断后立即抢锁

		// pthread_mutex_unlock(&me->mut);
		// sched_yield();
		// pthread_mutex_unlock(&me->mut);
	}
	n = min(me->token, size);

	me->token -= n; // 另外一个线程在每秒更新token值，这样就会产生冲突
	pthread_mutex_unlock(&me->mut);
	return n;

}

int mytbf_returntoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;

	if(size <= 0)
		return -EINVAL;
	pthread_mutex_lock(&me->mut);
	me->token += size;
	if(me->token > me->burst)
		me->token = me->burst;
	pthread_mutex_unlock(&me->mut);
	return size;
}

int mytbf_destroy(mytbf_t *ptr)
{
	struct mytbf_st *me = ptr;

	pthread_mutex_lock(&mut_job);
	job[me->pos] = NULL;
	
	pthread_mutex_unlock(&mut_job);

	pthread_mutex_destroy(&me->mut);
	pthread_mutex_destroy(&me->cond);
	free(ptr);

	return 0;

}
