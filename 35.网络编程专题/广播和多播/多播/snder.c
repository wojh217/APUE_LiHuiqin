#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "proto.h"

int main(int argc, char *argv[])
{
	int sd;
	struct msg_st sbuf;
	struct sockaddr_in raddr;


	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}


	struct ip_mreqn mreq;

	inet_pton(AF_INET, MULGROUP, &mreq.imr_multiaddr); // 多播组的地址
	inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address); // 自己的ip地址
	// ip ad sh
	// if_nametoindex()
	mreq.imr_ifindex = if_nametoindex("eth4"); //参数网卡名称

	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0 ) 
	{
		perror("setsockopt()");
		exit(1);
	}


	strcpy(sbuf.name, "Alan");
	sbuf.math = htonl(rand()%100);
	sbuf.chinese = htonl(rand()%100);

	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, MULGROUP, &raddr.sin_addr); // 给组内发消息


	if(sendto(sd, &sbuf, sizeof(sbuf), 0, (void *)&raddr, sizeof(raddr)) < 0 )
	{
		perror("sendto");
		exit(1);
	}

	puts("OK!");


	close(sd);

	exit(0);
}