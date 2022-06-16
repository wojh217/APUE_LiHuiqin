#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char **argv)
{

	struct passwd *pwdline;
	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	pwdline = getpwuid(atoi(argv[1])); // 返回的指针所指向的内容存储在哪上？堆or静态区？

	if(pwdline == NULL)
	{
		perror("getpwuid()");
		exit(1);
	}

	puts(pwdline->pw_name);

	exit(0);
}