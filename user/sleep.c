// path: user/sleep.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc <= 0) {
        printf("%s\n", "sleep: No argument!");
        //fprintf(2, "sleep: No argument!\n");
        exit(1);
    }

    int ticks = atoi(argv[1]);
    if (ticks < 0)    exit(1);
    int ret = sleep(ticks);
    if (ret < 0)    exit(1);

    exit(0);
}
