#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MIN 30000000
#define MAX 30000200
#define THRNUM 4

static int num = 0; // 初始值为0， -1表示结束
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_num = PTHREAD_COND_INITIALIZER;

static void * func(void *p);

// 求MIN到MAX之间的质数
int main()
{
        pthread_t tid[THRNUM];
        int i;

        for(i=0; i<THRNUM; i++)
        {
                pthread_create(tid+i, NULL, func, (void *)i);

        }

        // 放数，放数时是一个个放的，放一个消耗一个然后再放下一个
        for(i=MIN; i<=MAX; i++)
        {
                pthread_mutex_lock(&mut_num);
                while(num != 0)
                {
                        pthread_cond_wait(&cond_num, &mut_num);
                        // pthread_mutex_unlock(&mut_num);
                        // sched_yield(); // 出让调度器给其他程序
                        // pthread_mutex_lock(&mut_num);
                }
                num = i;
                pthread_cond_signal(&cond_num);
                pthread_mutex_unlock(&mut_num);
        }

        // 所有的数放完之后
        pthread_mutex_lock(&mut_num);
        while(num != 0)
        {
                pthread_cond_wait(&cond_num, &mut_num);
        }
        num = -1;
        pthread_cond_broadcast(&cond_num);
        pthread_mutex_unlock(&mut_num);


        for(i=0; i<THRNUM; i++)
        {
                pthread_join(tid[i], NULL);
        }

        pthread_mutex_destroy(&mut_num);
        pthread_cond_destroy(&cond_num);


        exit(0);
}

static void * func(void *p)
{
        int i, j;
        int primer_flag;

        while(1)
        {
                pthread_mutex_lock(&mut_num);
                while(num == 0)
                {
                        pthread_cond_wait(&cond_num, &mut_num);
                        // pthread_mutex_unlock(&mut_num);
                        // sched_yield(); // 出让调度器给其他程序
                        // pthread_mutex_lock(&mut_num);
                }

                // 跳出临界区外时要注意释放锁
                if(num == -1)
                {
                        pthread_mutex_unlock(&mut_num);
                        break;
                }

                i = num;
                num = 0;
                pthread_cond_broadcast(&cond_num);
                pthread_mutex_unlock(&mut_num);

                primer_flag = 1;

                for(j = 2; j <= i/2; j++)
                {
                        if(i % j ==0) // 能够整除，不是质数
                        {
                                primer_flag = 0;
                                break;
                        }
                }
                if(primer_flag == 1)
                {
                        printf("[%d]%d is primer\n", (int)p, i);
                }

        }
        pthread_exit(NULL);
}