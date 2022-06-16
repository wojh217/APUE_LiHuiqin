#include <stdio.h>
#include <stdlib.h>
#include <glob.h>

// #define PAT "/etc/a*.conf"
#define PAT "/etc/*" // 不包含隐藏文件，如果要查看隐藏用"/etc/.*"，且将会匹配到. 和..这两个特殊文件


static int errfunc (const char *epath, int eerrno)
{
	puts(epath);
	fprintf(stderr, "ERROR MSG: %S\n", strerror(eerrno));

}

int main()
{

	glob_t globres;
	int i, err;

	err = glob(PAT, 0, NULL, &globres);
	if(err)
	{
		printf("Error code = %d", err);
		exit(1);
	}

	for(i=0; i < globres.gl_pathc; i++)
	{
		puts(globres.gl_pathv[i]);
	}

	//释放申请的内存空间
	globfree(&globres);

	exit(0);
}