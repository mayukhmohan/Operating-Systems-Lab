#include<stdio.h>
#include<stdlib.h>
int fact(int n);
void answer(int n,int *arr);
int main(int argc, char *argv[])
{
        int i;
        int arr[argc];
        for(i=1;i<argc;i++)
        {
                arr[i] = (int)*argv[i] - 48;
        }

        /*for(i=1;i<argc;i++)
        {
                printf("%d\n",fact(arr[i]));
        }*/

        /*pid_t pid1, pid2;
        pid1 = fork();
        printf("First Parent\n");       

        if(pid1 > 0)
        {
		pid2 = fork();
                if(pid2 > 0)
                {
                        printf("%d second child\n",fact(arr[1]));
                }       
                else if(pid2 == 0)
                {
                        printf("%d second parent\n",fact(arr[2]));
                }
        }
        else if(pid1 == 0)
        {
                printf("%d first child\n",fact(arr[3]));
        }*/
        answer(argc-1,arr);
}
int fact(int n)
{
        if(n==0)
                return 1;
        else
        {
                return (n * fact(n-1));
        }
}
void answer(int n, int *arr)
{
        pid_t pid;
        if(n==1)
        {
                printf("%d\n",fact(arr[n]));
                return;
        }
        pid = fork();
        if(pid==0)
        {
                answer(n-1,arr);
                printf("%d\n",fact(arr[n]));
                exit(0);
        }
        else
                wait(NULL);
}

