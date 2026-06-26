#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(2, "sixfive: need to pass at least one argument\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            printf("sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }
        char buf;
        int x = 0, flag1 = 0, flag2 = 0;
        while (1) {
            int n = read(fd, &buf, 1);
            printf("%d\n", n);
            if (n == 0) {
                if (flag1 && !flag2 && (x % 5 == 0 || x % 6 == 0)) {
                    printf("%d\n", x);
                }
                break;
            }
            if (buf == ' ' || buf == '-' || buf == '\r' || buf == '\t' 
                || buf == '\n' || buf == '.' || buf == '/' || buf == ',') {
                if (flag1 && !flag2 && (x % 5 == 0 || x % 6 == 0)) {
                    printf("%d\n", x);
                }
                x = flag1 = flag2 = 0;
            }
            else if (buf >= '0' && buf <= '9') {
                flag1 = 1;
                x = x * 10 + buf - '0';
            }
            else {
                flag2 = 1;
            }
        }
    }
    exit(0);
}