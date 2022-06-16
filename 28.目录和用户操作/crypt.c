#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <shadow.h>
#include <string.h>


// crypt函数需要定义_XOPEN_SOURCE宏
int main(int argc, char **argv)
{

	char *input_pass;
	struct spwd* shdowline;
	char *crypted_pwd;

	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	// 获取输入的密码
	input_pass = getpass("Enter Passwd:");
	
	// 获取口令原文，只有root账户有权限
	shdowline = getspnam(argv[1]);


	// 加密
	crypted_pwd = crypt(input_pass, shdowline->sp_pwdp);

	if(strcmp(crypted_pwd,shdowline->sp_pwdp) == 0)
	{
		printf("password succ.\n");
	}
	else{
		printf("password fail.\n");
	}

	

	exit(0);
}