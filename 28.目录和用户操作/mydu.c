#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>

#define PAHTSIZE 1024


static int path_noloop(const char *path)
{
	// /aa/bb/cc/dd/..

	char *pos;
	pos = strrchr(path, '/'); // 最后一次出现字符'/'的位置

	if(pos == NULL)
		exit(1);


	if(strcmp(pos+1, ".") == 0 || strcmp(pos+1, "..") == 0)
		return 0;
	else
		return 1;
}

// 返回一个64位的数表示目录的大小
// 路径可能是相对/绝对路径
static int64_t mydu(const char *path)
{
	// /aa/bb/cc/dd

	static struct stat statres;
	static char nextpath[PAHTSIZE];
	glob_t globres;

	int i, err;
	int64_t totalsize;

	if(lstat(path, &statres) < 0)
	{
		perror("lstat()");
		exit(1);

	}

	// 目录形式， 查找其下的文件
	// 使用aaa/* aaa/.*匹配模式找到所有文件
	if(S_ISDIR(statres.st_mode))
	{
		memset(nextpath, 0, sizeof(nextpath));
		strncpy(nextpath, path, PAHTSIZE);
		strncat(nextpath, "/*", PAHTSIZE); // 字符串的拼接

		err = glob(nextpath, 0, NULL, &globres); //未匹配到时globres.gl_pathc为0
		if(err == GLOB_NOSPACE || err == GLOB_ABORTED)
		{
			printf("Error1 code = %d\n", err);
			exit(1);
		}

		strncpy(nextpath, path, PAHTSIZE);
		strncat(nextpath, "/.*", PAHTSIZE); // 字符串的拼接
		err = glob(nextpath, GLOB_APPEND, NULL, &globres); // 追加模式
		if(err == GLOB_NOSPACE || err == GLOB_ABORTED)
		{
			printf("Error2 code = %d\n", err);
			exit(1);
		}


		totalsize = statres.st_blocks;  //自身目录的大小
		// 找到其下的所有文件
		for(i=0; i<globres.gl_pathc; i++) // i横跨了递归点
		{
			if(path_noloop(globres.gl_pathv[i]))
			{
				totalsize += mydu(globres.gl_pathv[i]); // 递归点
			}
		}

		globfree(&globres);
		return totalsize;
	}

	else{
		// 非目录，直接返回st_blocks/2
		return statres.st_blocks;
	}
	
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	printf("%lld\n", mydu(argv[1]) / 2);
}