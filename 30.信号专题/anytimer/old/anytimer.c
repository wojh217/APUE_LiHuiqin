#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include "mytbf.h"

typedef void(*sighandler_t)(int);  // 自定义信号处理函数函数指针类型

static struct mytbf_st* job[MYTBF_MAX]; //存放不同实例的数组
static int inited = 0; 					// 确定是否第一次创建实例
static sighandler_t alrm_handler_save;  // 保存原alarm的信号处理行为


struct mytbf_st
{
	int cps; //每秒要处理(即read)的字节数
	int burst; 	// 令牌总数的最大值，其令牌数不能超过该最大值
	int token; // 令牌总数，即收到信号+cps,处理一次减去cps
	int pos;  // 该任务组在数组中的下标位置
};


static void alrm_handler(int s)
{
	int i;
	alarm(1);

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
	signal(SIGALRM, alrm_handler_save); // 这里不能用sigdefault，而用原来的信号处理函数
	//恢复
	alarm(0); // 这里是什么含义？ (由于alarm的再次使用，会取消之前所有的alarm，且alarm(0)不会产生新闹钟，所有相当于关掉闹钟)
	for(i = 0; i < MYTBF_MAX; i++)
		free(job[i]);
}

static void module_load(void )
{
	alrm_handler_save = signal(SIGALRM, alrm_handler); // alrm_handler_save保存原来的信号处理行为
	alarm(1);

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
