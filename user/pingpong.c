// path: user/pingpong.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p[2];
    char buf[32];

    pipe(p);
    if (fork() > 0) {                           // 父进程
	    int pid = getpid();
        int n = write(p[1], "1", 1);
        if (n != 1)    exit(1);
        close(p[1]);                            // 写完关闭写端
        n = read(p[0], buf, sizeof(buf));       // 读是阻塞操作，除非子进程写入数据
        if (n < 0)    exit(1);
	    printf("%d: received pong\n", pid);
        close(p[0]);
    } else {
	    int pid = getpid();
        int n = read(p[0], buf, sizeof(buf));   // 读是阻塞操作，除非父进程写入数据
        if (n < 0)    exit(1);
        close(p[0]);
        printf("%d: received ping\n", pid);
        n = write(p[1], "1", 1);
        if (n != 1)    exit(1);
        close(p[1]);
    }

    exit(0);
}
