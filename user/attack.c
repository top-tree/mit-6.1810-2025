#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

static int match(char *p) {
  return p[0]  == 'T' &&
         p[1]  == 'h' &&
         p[2]  == 'i' &&
         p[3]  == 's' &&
         p[4]  == ' ' &&
         p[5]  == 'm' &&
         p[6]  == 'a' &&
         p[7]  == 'y' &&
         p[8]  == ' ' &&
         p[9]  == 'h' &&
         p[10] == 'e' &&
         p[11] == 'l' &&
         p[12] == 'p' &&
         p[13] == '.' &&
         p[14] == '\0';
}

static int check(char c) {
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z');
}

int
main(int argc, char *argv[])
{
  int size = 32 * PGSIZE;
  char *p = sbrk(size);

  if (p == (char *)-1) {
    exit(1);
  }

  for (int i = 0; i + 16 < size; i++) {
    if (match(p + i)) {
      char *secret = p + i + 16;
      int len = 0;

      while (i + 16 + len < size && check(secret[len])) {
        len++;
      }

      if (len > 0) {
        write(1, secret, len);
        write(1, "\n", 1);
        exit(0);
      }
    }
  }

  exit(1);
}