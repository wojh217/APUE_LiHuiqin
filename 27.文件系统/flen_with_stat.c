#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// static修饰函数，禁止外部扩展
// 文件返回大小，用int还是off_t哪个准确？
static int flen(const char *fname)
{
	struct stat statres;
	 // 传递结构体变量的地址
	if(stat(fname, &statres) < 0)
	{
		perror("stat()");
		exit(1);
	}

	return statres.st_size; // st_size的类型为off_t， off_t类型的大小不同的编译器不同, 可能16、32、64位
}

int main(int argc, char **argv)
{

	if(argc < 2)
	{
		fprintf(stderr, "Usage..\n");
		exit(1);
	}

	// off_t类型的大小不同的编译器不同， 因此可以在此处强转为long long再打印
	// 也可以在编译的makefile文件中指定选项CFLAGS+=-D_FILE_OFFSET_BITS=64, 来使off_t称为64位
	printf("%lld\n", (long long)flen(argv[1])); 
	                                            

	exit(0);
}