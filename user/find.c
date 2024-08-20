// path: user/find.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// ls.c 里已经定义，且没被 static 修饰，所以直接引入
extern char *fmtname(char *);

void
find(char *dir, char *path)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(dir, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }

    // 第二个参数不是目录(dir)，ERROR
    if (st.type == T_FILE) {
        fprintf(2, "find: %s is not a dir\n", dir);
        close(fd);
        return;
    }

    // 封装为绝对路径，存入 buf
    memmove(buf, dir, strlen(dir));
    p = buf + strlen(dir);
    *p++ = '/';
    if (st.type == T_DIR) {// 只处理形参 dir 为目录的情况(还可以是设备，但这里不考虑)
        // 遍历当前目录下所有文件/目录
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)    continue;
            if (strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0)    continue;

            // 封装为绝对路径，即此时 buf = dir + 当前迭代读出的文件/目录名
            memmove(p, de.name, strlen(de.name));
            p[strlen(de.name)] = '\0';

            if (stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            if (st.type == T_DIR) {// 如果读出是目录，递归
                find(buf, path);
                continue;
            }

            if (strcmp(path, de.name) == 0) {// 如果读出是文件，判相等
                printf("%s\n", buf);
            }
        }
    }

    close(fd);
}

int
main(int argc, char *argv[])
{
    if(argc < 3) {
        fprintf(2, "find: cannot exec with too few parameters\n");
        exit(1);
    }
    for(int i = 2;  i < argc; i++)//可能有这种情况 find . file1 file2 file3
        find(argv[1], argv[i]);
    exit(0);
}
