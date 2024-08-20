// path: user/primes.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p[2] = {0}, buf[64] = {0}, size = sizeof(int);

    // 第一个进程只需要 feed 所有数字然后等待即可
    pipe(p);
    for (int i = 2; i <= 35; ++i)
        if (write(p[1], &i, size) != size)    exit(1);
    while (1) {
        if (fork() == 0) {                      // 子进程
            close(p[1]);
            int n = 0, prime = 0, ptr = 0, tmp = 0;
            n = read(p[0], &prime, size);
            if (n == 0)    exit(0);             // 第一个字都读不出来就直接返回
            if (n < 0)    exit(1);
            printf("prime %d\n", prime);

            for (int i = 0; n > 0; ++i) {
                n = read(p[0], &tmp, size);
                if (n < 0)    exit(1);

                if (tmp % prime)    buf[ptr++] = tmp;
            }

            close(p[0]);
            pipe(p);                            // 开启另一个 pipe
            for (int i = 0; i < ptr; ++i)
                if (write(p[1], &buf[i], size) != size)    exit(1);
        } else {
            // 父进程，不止第一个进程会进来
            // 当第二个进程创建子进程后就变成了父进程
            // 此时该进程也会进来
            // 但无论怎样都好，反正父进程都让它等待即可
            close(p[0]);
            close(p[1]);
            wait(0);
            exit(0);
        }
    }
}
