#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MIN 30000000
#define MAX 30000200
#define N 3 // 进程数

// 回收已创建的子进程，然后主进程退出(如果子进程正在运行怎么办？会等待其运行结束还是直接都结束？)
// 当父进程退出前使用wait，其子进程还未结束，会被父进程结束吗？

int main()
{
        pid_t pid;
        int i,j,n,m;
        int primer_flag;

        for(n=0; n<N; n++)
        {
            pid = fork();
            if(fork < 0)  //fork失败，回收已创建的子进程，然后主进程退出(如果子进程正在运行怎么办？会等待其运行结束还是直接都结束？)
            {
                perror("fork()");
                for(m=0; m<n; m++) // 创建的进程编号已经到n-1
                {
                    wait(NULL);
                }
                exit(1);
            }
            else if(pid == 0)
            {
                for(i=MIN+n; i<=MAX; i+=N) // 隔N个取一个数
                {
                    primer_flag = 1;
                    for(j = 2; j < i/2; j++)
                    {
                            if(i % j ==0) // 能够整除，不是质数
                            {
                                    primer_flag = 0;
                                    break;
                            }
                    }
                    if(primer_flag == 1)
                    {
                            printf("[%d] %d is primer\n", n, i);
                    }
                }
                exit(0);
            }
        }

        for(n=0; n<N; n++)
        {
           wait(NULL);
        }

        exit(0);
}