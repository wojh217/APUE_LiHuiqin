#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MIN 30000000
#define MAX 30000200

// 求MIN到MAX之间的质数
int main()
{
        pid_t pid;
        int i,j;
        int primer_flag;

        for(i=MIN; i<=MAX; i++)
        {
            pid = fork();
            if(pid == 0) // 子进程执行判断操作
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
                            printf("%d is primer\n", i);
                    }
                    sleep(1000); // 子进程最后退出，父进程先退出
                    exit(0); // 子进程执行结束后，exit退出进程，否则也将执行循环语句 
            }
                

        }
        for(i=MIN; i<=MAX; i++)
        {
           wait(NULL);  // 只wait一次，能回收所有子进程吗 不行，wait是有一个子进程状态发生变化就返回，如果有n个子进程结束，就要调用n次wait   
        }

        // 如果父进程sleep子进程执行完后没有sleep,则子进程先执行完，且由于未被父进程回收称为僵尸进程，僵尸进程将占用进程号
        // 如果父进程没有sleep直接执行完，而子进程sleep，则父进程先执行结束退出，子进程由init进程接管，称为s态
        // 如果父子进程都执行sleep，则通过ps axf查看进程情况，是shell进程创建父进程，父进程创建子进程
        exit(0);
}