#ifndef MYPIPI_H__
#define MYPIPI_H__

#define PIPISIZE 1024
#define MYPIPE_READ  0x00000001UL
#define MYPIPE_WRITE 0x00000002UL

typedef void mypipe_t 

mypipe_t *mypite_init(void);


int mypipe_register(mypipe_t *, int opmap);

int mypipe_unregister(mypipt_t *, int opmap);

int mypite_read(mypipe_t *, void *buf, size_t count);  //仿照系统io

int mypite_write(mypipe_t *, const void *buf, size_t count);

int mypite_destroy(mypipe_t *);


#endif