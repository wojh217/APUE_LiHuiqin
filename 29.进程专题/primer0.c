#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN 30000000
#define MAX 30000200

// 求MIN到MAX之间的质数
int main()
{
        pid_t pid;

        printf("[%d]: Begin!\n", getpid());

        fflush(NULL);

        int i,j;
        int primer_flag;


        for(i=MIN; i<=MAX; i++)
        {
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
                        printf("%d is primer\n", i);
                }

                

        }

        printf("[%d]: End!\n", getpid());

        exit(0);
}