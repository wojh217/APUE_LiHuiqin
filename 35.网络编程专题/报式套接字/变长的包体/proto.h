#ifndef PROTO_H__
#define PROTO_H__

#define RCVPORT  "1989"
#define NAMEMAX  (20-8-8-1) //因为name是str，最后一位必须是'\0'，因此name的最大长度是数组的最大长度减一
#define ARRAYMAX (20-8-8)

// 变长结构体
// 512是udp包的推荐长度, udp包头有8个字节，math和chinese也一共占用8个字节
struct msg_st 
{
	uint32_t math;
	uint32_t chinese;
	uint8_t name[1];  // 这里只是一个占位符，参考数据结构中变长结构体的使用
}__attribute__((packed));   // 不对齐


#endif