#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
        pid_t pid;

        printf("[%d]: Begin!\n", getpid());

        fflush(NULL);

        pid = fork();
        if(pid<0)
        {
                perror("fork()");
                exit(1);
        }
        else if(pid == 0)
        {
                printf("[%d]: Child is working!\n", getpid());
        }
        else
        {
                printf("[%d]: Parent is working!\n", getpid());
        }


        printf("[%d]: End!\n", getpid());

        exit(0);
}