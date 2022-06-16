#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define RCVPORT  "1989"

int main()
{
	uint32_t m, n;
	uint32_t bind_addr;
	struct hostent* h;
	struct sockaddr_in laddr;

	m = 100;

	printf("m: %u\n", m);

	n = htonl(m);

	printf("n: %u\n", n);

	

	printf("INADDR_ANY: %u\n", INADDR_ANY);
	printf("INADDR_ANY_2: %u\n", htonl(INADDR_ANY));


	printf("INADDR_LOOPBACK: %u\n", INADDR_LOOPBACK);         // 2130706433
	printf("INADDR_LOOPBACK: %u\n", htonl(INADDR_LOOPBACK));  // 16777343

	inet_pton(AF_INET, "127.0.0.1", &bind_addr);

	printf("bind_addr: %u\n", bind_addr);					 // 16777343
	printf("bind_addr_2: %u\n", htonl(bind_addr));			 // 2130706433



	printf("bind_port: %u\n", atoi(RCVPORT));
	printf("bind_port_2: %u\n", htons(atoi(RCVPORT)) );


	h = gethostbyname("127.0.0.1");
	printf("h->addr: %u\n", h->h_addr);


	inet_pton(AF_INET, "127.0.0.1", &laddr.sin_addr);
	printf("laddr: %u\n", laddr.sin_addr.s_addr);


	exit(0);	

}
