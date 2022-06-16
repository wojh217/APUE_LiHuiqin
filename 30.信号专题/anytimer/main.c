#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "anytimer.h"

static void f1(char *p)
{
    printf("f1(): %s\n", p);

}

static void f2(char *p)
{
    printf("f2(): %s\n", p);
    sleep(100);

}

int main()
{
    // begin!end!...bbb...aaa...ccc................
    int job1, job2, job3;

    puts("begin!");
//    job1 = at_addjob(5, f1, "aaa");
    job1 = at_addjob_repeat(5, f1, "aaa");
    if(job1 < 0)
    {
        fprintf(stderr, "at_addjob():%s\n", strerror(-job1));
        exit(1);
    }

//    at_addjob(2, f2, "bbb");
//    at_addjob(7, f1, "ccc");

    puts("end!");


    while(1)
    {
        write(1, ".", 1);
        sleep(1);
    }

    exit(0);
}