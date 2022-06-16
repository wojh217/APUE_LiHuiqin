#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TRUNCSIZE 20
#define FNAME "/tmp/out"
#define LINESIZE 1024


static void * func(void *p);

// 求MIN到MAX之间的质数
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

        fgets(linebuf, LINESIZE, fp);

        puts(linebuf);
        fseek(fp, 0, SEEK_SET);

        sleep(1);
        fprintf(fp, "%d\n", atoi(linebuf) + 1);


        fclose(fp);
        pthread_exit(NULL);
}