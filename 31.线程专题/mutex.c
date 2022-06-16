#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TRUNCSIZE 20
#define FNAME "/tmp/out"
#define LINESIZE 1024

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static void * func(void *p);

// 多个线程打开文件读取内容，将内容值加一后回写
int main()
{
        pthread_t tid[TRUNCSIZE];

        int i;

        for(i=0; i<TRUNCSIZE; i++)
        {
                pthread_create(&tid[i], NULL, func, NULL);

        }

        for(i=0; i<TRUNCSIZE; i++)
        {
                pthread_join(tid[i], NULL);
        }


        pthread_mutex_destroy(&mut); // 销毁互斥量


        exit(0);
}

static void * func(void *p)
{
        FILE *fp;
        char linebuf[LINESIZE];

        
        fp = fopen(FNAME, "r+");
        if(fp == NULL)
        {
                perror("fopen()");
                exit(1);
        }

        // 文件可以同时打开，但读和写不能同事进行

        // 这也说明文件打开只是生成一个结构体，打开的文件如果被其他程序所修改，则会立即反应到此结构体上
        // 所以多个程序打开同一文件时，一个程序可以立即感知另外程序对此文件的修改
        // 那如果文件销毁，也会立即感知到吗？比如一方删除此文件，此时一个程序仍再读或写，会发生什么？

        pthread_mutex_lock(&mut);
        fgets(linebuf, LINESIZE, fp); // 读操作

        puts(linebuf);
        fseek(fp, 0, SEEK_SET);

        sleep(1); // 
        fprintf(fp, "%d\n", atoi(linebuf) + 1);
        fclose(fp);

        pthread_mutex_unlock(&mut);

        pthread_exit(NULL);
}