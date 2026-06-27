#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void run_exec(char **cmd, int cmd_argc, char *file) {
    if (cmd_argc + 2 > MAXARG) {
        fprintf(2, "find: too many args\n");
        exit(1);
    }
    char *exec_argv[MAXARG];
    for (int i = 0; i < cmd_argc; i++) {
        exec_argv[i] = cmd[i];
    }
    exec_argv[cmd_argc] = file;
    exec_argv[cmd_argc + 1] = 0;
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "find: fork failed\n");
        exit(1);
    }
    if (pid == 0) {
        // child
        exec(exec_argv[0], exec_argv);
        // failed
        fprintf(2, "find: exec %s failed\n", exec_argv[0]);
        exit(1);
    }
    else {
        // parent
        wait(0);
    }
}

void find(char *path, char *target, int use_exec, char **cmd, int cmd_argc) {
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
        close(fd);
        if (strcmp(name + 1, target) == 0) {
            if (use_exec) {
                run_exec(cmd, cmd_argc, path);
            }
            else {
                printf("%s\n", path);
            }
        }
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
            find(buf, target, use_exec, cmd, cmd_argc);
        }
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3 && (argc < 5 || strcmp(argv[3], "-exec") != 0)) {
        fprintf(2, "usage: find path name [-exec cmd ...]\n");
        exit(1);
    }
    int use_exec = 0;
    char **cmd = 0;
    int cmd_argc = 0;
    if (argc >= 5) {
        use_exec = 1;
        cmd = &argv[4];
        cmd_argc = argc - 4;
    }
    find(argv[1], argv[2], use_exec, cmd, cmd_argc);
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