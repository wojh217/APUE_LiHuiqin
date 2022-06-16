#ifndef PROTO_H__
#define PROTO_H__

// KEYPROJ是一个整数
#define KEYPATH "/etc/services"
#define KEYPROJ 'a'

#define PAHTMAX 1024
#define DATAMAX 1024


// 传输的数据


enum
{
	MSG_PAHT = 1;
	MSG_DATA,
	MSG_EOT
};

typedef struct msg_path_st
{
	long mtype;         // must be MSG_PAHT
	char path[PAHTMAX]; // ASCIIZ带尾0的串
}msg_path_t;

typedef struct msg_data_st
{
	long mtype;         // must be MSG_DATA
	char data[DATAMAX];
	int datalen;
}msg_data_t;

typedef struct msg_eot_st
{
	long mtype;         // must be MSG_EOT
}msg_eot_t;


union msg_s2c_un
{
	long mttype;       // mttype是msg_data_t和msg_eot_t的共有变量，因此可以提取出来
	msg_data_t datamsg;
	msg_eot_t eotmsg
}

#endif