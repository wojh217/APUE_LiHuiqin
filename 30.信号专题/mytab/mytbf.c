#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "mytbf.h"


// 将ararm、signal用setitimer、sigaction替换
static struct mytbf_st* job[MYTBF_MAX]; //存放不同实例的数组
static int inited = 0; 					// 确定是否第一次创建实例


static struct sigaction alrm_sa_save;

struct mytbf_st
{
	int cps; //每秒要处理(即read)的字节数
	int burst; 	// 令牌总数的最大值，其令牌数不能超过该最大值
	int token; // 令牌总数，即收到信号+cps,处理一次减去cps
	int pos;  // 该任务组在数组中的下标位置
};


static void alrm_action(int s, siginfo_t *infop, void *unused)
{
	int i;

	// 判断信号的来源
	if(infop->si_code != SI_KERNEL)
		return;

	//每个组元素的token增加
	for(i = 0; i < MYTBF_MAX; i++)
	{
		if(job[i] != NULL)
		{
			job[i]->token += job[i]->cps;
			if(job[i]->token  > job[i]->burst)
				job[i]->token  = job[i]->burst;
		}
	}
}

static void module_unload(void)
{
	int i;
	struct itimerval itv;


	sigaction(SIGALRM, &alrm_sa_save, NULL);

	// 关闭时钟
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, NULL);


	for(i = 0; i < MYTBF_MAX; i++)
		free(job[i]);
}

static void module_load(void )
{

	struct sigaction sa;
	struct itimerval itv;

	sa.sa_sigaction = alrm_action;
	sigempty(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;  // 表示是sa_sigaction

	sigaction(SIGALRM, &sa, &alrm_sa_save);
	// if error


	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;

	setitimer(ITIMER_REAL, &itv, NULL);
	// if error

	atexit(module_unload); //钩子函数，这里时候时候会执行钩子函数？
}



static int get_free_pos(void)
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

	if(!inited)
	{
		module_load(); //由于alarm信号不同多次使用，因此只有第一次使用init函数时才会发出信号
		inited = 1;
	}

	pos = get_free_pos();
	if(pos < 0)
		return NULL;

	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	me->pos = pos;

	job[pos] = me;

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
	// 信号到来时才可以获取，否则将阻塞
	while(me->token <= 0)
		pause();
	n = min(me->token, size);

	me->token -= n;
	return n;

}

int mytbf_returntoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;

	if(size <= 0)
		return -EINVAL;

	me->token += size;
	if(me->token > me->burst)
		me->token = me->burst;

	return size;
}

int mytbf_destroy(mytbf_t *ptr)
{
	struct mytbf_st *me = ptr;
	job[me->pos] = NULL;
	free(ptr);

	return 0;

}
