#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "proto.h"

#define IPSTRSIZE 40

int main()
{
	int sd;
	struct sockaddr_in laddr, raddr;
	struct msg_st rbuf;
	socklen_t raddr_len;
	char ipstr[IPSTRSIZE];

	sd = socket(AF_INET, SOCK_DGRAM, 0); // 0表示默认，即AF_INET中使用SOCK_DGRAM的协议，即IPPROTO_UDP
	if(sd<0)
	{
		perror("socket()");
		exit(1);
	}


	struct ip_mreqn mreq;
	inet_pton(AF_INET, MULGROUP, &mreq.imr_multiaddr); // 多播组的地址
	inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address); // 自己的ip地址
	mreq.imr_ifindex = if_nametoindex("eth4"); //参数网卡名称

	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0 ) 
	{
		perror("setsocket()");
		exit(1);
	}
	
	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(RCVPORT)); // 这里由于端口号也需要传输，因此要换为网络字节序？
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
	

	// bind要求的是struct sockaddr类型的地址， 这里将地址强转为void*,
	// 或者强转为struct sockaddr *， 后续操作此地址会有问题吗？
	// 比如struct sockaddr_in 和 struct sockaddr长度是否一致，这样强行转后使用变量时是否会出错？
	if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
	{
		perror("bind()");
		exit(1);
	}

	/*!!!*/
	// 这里raddr_len一定要初始化
	// 如果不初始化，第一次打印的地址会出错，之后才会正常， 为社么呢
	// 因为第一次是按照raddr_len大小长度来取的地址，如果长度为0，则获取不到？
	// 而之后raddr_len值被改变

	raddr_len = sizeof(raddr);

	while(1)
	{
		// 流式用recv，报式用recvfrom，流式比报式少了地址信息和端口号，因为流式已经一对一建立连接
		// ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
		// ssize_t recv(int sockfd, void *buf, size_t len, int flags);

		// buf 接收数据的空间 len空间的大小 flags特殊标记 src_addr对端的地址 addrlen 对端的地址长度
		// 对端和接收端都是ipv4协议
		// 接收时将raddr回填
		recvfrom(sd, &rbuf, sizeof(rbuf), 0, (void *)&raddr, &raddr_len);

		// 将大整数转化为点分式
		// const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
		inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
		printf("--MESSAGE FROM %s:%d---\n", ipstr, ntohs(raddr.sin_port));
		printf("NAME = %s\n", rbuf.name); // 单字节传输，不用host和net的转换
		printf("MATH = %d\n", ntohl(rbuf.math));
		printf("CHINESE = %d\n", ntohl(rbuf.chinese));
	}
	

	// 运行起来后用netstat -anu 查看udp
	close(sd);


	exit(0);
}