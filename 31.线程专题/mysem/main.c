#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mysem.h"

#define MIN 30000000
#define MAX 30000200
#define TRUNCSIZE (MAX-MIN+1)
#define N   4
static mysem_t *sem;


static void * func(void *p);

// 求MIN到MAX之间的质数
int main()
{
        pthread_t tid[TRUNCSIZE];

        int i;

        sem = mysem_init(N);
        if (sem == NULL)
        {
                fprintf(stderr, "mysem_init() failed! \n");
                exit(1);
        }


        for(i=MIN; i<=MAX; i++)
        {
                mysem_sub(sem, 1);
                pthread_create(&tid[i-MIN], NULL, func, (void *)i);

        }

        for(i=MIN; i<=MAX; i++)
        {
                pthread_join(tid[i-MIN], NULL);
        }

        mysem_destroy(sem);

        exit(0);
}

static void * func(void *p)
{
        int i, j;
        int primer_flag = 1;

        i = (int)p;

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
                printf("%d is primer\n", i);
        }

        sleep(5); // 为了用ps查看

        mysem_add(sem, 1);
        pthread_exit(NULL);
}