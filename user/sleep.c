#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(2, "Error: need to pass an argument\n");
        exit(1);
    }
    if (argc >= 3) {
        fprintf(2, "Error: only need one argument\n");
        exit(1);
    }
    int t = atoi(argv[1]);
    pause(t);
    exit(0);
}