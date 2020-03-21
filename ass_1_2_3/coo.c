#include <stdio.h>
#include <unistd.h>
int main () {
  int real = getuid();
  int euid = geteuid();
  int gid = getgid();
  int egid = getegid();
  printf("The REAL UID =: %d\n", real);
  printf("The EFFECTIVE UID =: %d\n", euid);
  printf("The Group ID =: %d\n",gid);
  printf("The Effective Group ID =: %d\n",egid);
  printf("World Hello\n");
}
