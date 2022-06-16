#include <stdio.h>
#include <stdlib.h>


int main()
{

	// 多个alarm写到一起，最后一个生效
	alarm(10);
	alarm(1);
	alarm(5); 

	// 不使用pause，会忙等，cpu会飙满
	// while(1);

	while(1)
	{
		pause(); //人为设置一个系统阻塞等待信号来打断它，直到收到信号才会触发信号的默认行为或设置的信号处理函数
	};

	exit(0);

}


