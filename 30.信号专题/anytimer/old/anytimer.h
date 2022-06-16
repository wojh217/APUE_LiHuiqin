#ifndef ANYTIMER_H__
#define ANYTIMER_H__

#define JOB_MAX 1024


//usr/local/include/
typedef void at_jobfunc_t(void *);


int at_addjob (int sec, at_jobfunc_t *jobp, void *arg);
/*
 成功 > 0 
 参数错误 -EINVAL
 数组满 -ENOSPC
 内存空间不足 -ENOMEM
*/



int at_canceljob(int id);
/*
	任务状态running、overed、canceled
 成功 = 0 
 参数非法 -EINVAL
 失败，指定任务已完成 -EBUSY
 失败，指定任务重复取消 -ECANCELED 

	
*/


int at_waitjob(int id);
/*
	指定任务成功释放 0
*/

at_pausejob();

at_resumejob();

#endif