#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"

void find(char *path, char *target) {
    // printf("now: %s\n", path);
    char buf[512];
    struct stat st;
    struct dirent de;
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        exit(1);
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        exit(1);
    }
    if (st.type == T_FILE || st.type == T_DEVICE) {
        char *name = path + strlen(path);
        while (name >= path && *name != '/') {
            name--;
        }
        if (strcmp(name + 1, target) == 0) {
            printf("%s\n", path);
        }
        close(fd);
        return;
    }
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        exit(1);
    }
    strcpy(buf, path);
    char *p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = '\0';
        if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0) {
            continue;
        }
        if (st.type == T_DIR) {
            find(buf, target);
        }
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc != 3){
        fprintf(2, "usage: find path name\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}

/*
echo > b
mkdir a
echo > a/b
mkdir a/aa
echo > a/aa/b

find . b
*/