#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h> 
int main()
{
	printf("I'm exec1\n");
	printf("Process PID: %d\n", getpid());
	printf("Process Parent PID: %d\n", getppid());	
	return 0;
}
