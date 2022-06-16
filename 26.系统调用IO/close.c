#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {

    int fd;
    pid_t pid;
    char buf[20];
    ssize_t readnum;

    fd = open("/tmp/zhang/nn.txt", O_RDWR, 0666);
    if (fd < 0) {
        perror("open(src)");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        printf("[%d]: Child is working!\n", getpid());
        // 子进程等父进程close fd后，再进行读操作

        sleep(1);
        readnum = read(fd, buf, 20);
        if (readnum < 0) {
            perror("read()");

            exit(1);
        }
        printf("%s\n", buf);

    } else {
        // 父进程关闭fd
        printf("[%d]: Parent is working!\n", getpid());
        close(fd);
        sleep(5); // 父进程多等几秒
    }

    sleep(2);
    printf("[%d]: End!\n", getpid());

    exit(0);
}