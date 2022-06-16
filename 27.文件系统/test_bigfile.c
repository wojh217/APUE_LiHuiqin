#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



int main(int argc, char **argv)
{

	int fd;
	if(argc < 2)
	{
		fprintf(stderr, "Usage..\n");
		exit(1);
	}

	fd = open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, 0600);
	if(fd < 0)
	{
		perror("open()");
		exit(1);
	}
	
	          
    // 最好也判断lseek的返回值
    // 这里会报溢出警告，因为默认是int类型？
	// lseek(fd, 5 * 1024 * 1024 * 1024 - 1, SEEK_SET);
	lseek(fd, 5LL * 1024LL * 1024LL * 1024LL - 1LL, SEEK_SET);
	write(fd, "", 1); // 这里如果不写入，文件内容全为空洞，不占用磁盘空间

	close(fd);

	// 此文件创建后，使用stat查看，其Blocks块数为8，则每块521字节的话，共占用磁盘空间4K字节大小
	// 使用cp命令拷贝此文件，视频中拷贝出的文件Block数为0， 但我自己测试在centos6上为0， 在191服务器上仍为8
	// cp命令拷贝时，如果本次拷贝的内容全为0字符，则不进行写入，记录下读取的长度，继续读取，直到碰到有效数据后在
	// 新文件中执行lseek再进行写入操作，这样其中的空洞数据会自动生成，减少写入操作次数，节省操作时间
	exit(0);
}