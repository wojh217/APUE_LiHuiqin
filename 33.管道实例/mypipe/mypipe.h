#ifndef MYPIPI_H__
#define MYPIPI_H__

#define PIPISIZE 1024
#define MYPIPE_READ   0x000000001UL // unsigned long型位图
#define MYPIPE_WRITE  0x000000002UL //


typedef void mypipe_t 

mypipe_t *mypite_init(void);

int mypite_register(mypipe_t *, int opmap);

int mypite_unregister(mypipe_t *, int opmap);

int mypite_read(mypipe_t *, void *buf, size_t count);  //仿照系统io

int mypite_write(mypipe_t *, const void *buf, size_t count);

int mypite_destroy(mypipe_t *);


#endif