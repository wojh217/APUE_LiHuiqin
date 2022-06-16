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

typedef struct msg_s2c_st
{
	long mtype;         // must be MSG_DATA
	int datalen;        // 可以约定datalen>0时，是data包; 等于0时，是eot包，这样mtype也可以省略
	char data[DATAMAX];

}msg_data_t;


#endif