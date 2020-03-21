#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    char mode[] = "4755";
    char buf[100] = "/home/mayukh/mayukh_lab/foo";
    int i;
    i = strtol(mode, 0, 8);
    if (chmod (buf,i) < 0)
    {
        fprintf(stderr, "%s: error in chmod(%s, %s) - %d (%s)\n",
                argv[0], buf, mode, errno, strerror(errno));
        exit(1);
    }
    char mode1[] = "2755";
    char buf1[100] = "/home/mayukh/mayukh_lab/coo";
    int j;
    j = strtol(mode1, 0, 8);
    if (chmod (buf1,j) < 0)
    {
        fprintf(stderr, "%s: error in chmod(%s, %s) - %d (%s)\n",
                argv[0], buf1, mode1, errno, strerror(errno));
        exit(1);
    }
    return(0);
}
