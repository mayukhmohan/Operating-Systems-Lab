#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<pthread.h>

void *handler(pthread_t *b);
pthread_t arr[3];
int counter = 0;
int b=0,count_arr[3];
pthread_mutex_t lock;
pthread_barrier_t ba;

int main(int argc, char *argv[])
{
	int i;
	for(i=0;i<3;i++)
		count_arr[i] = 0;
	pthread_t first,second,third;
	if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\nMutex init has failed\n"); 
        return 0; 
    } 
    pthread_barrier_init(&ba,NULL,3);
	int b = 0;
	pthread_create((pthread_t *)&first,NULL,handler,&first);	
	pthread_create((pthread_t *)&second,NULL,handler,&second);
	pthread_create((pthread_t *)&third,NULL,handler,&third);
	pthread_join(first, 0);
	pthread_join(second, 0);
	pthread_join(third, 0);
	printf("\n------Results------\n");
	for(i=0;i<3;i++)
	{
		printf("\nThread #%d: %lu , increment: %d\n",i,arr[i],count_arr[i]);
	}
	return 0;
}

void *handler(pthread_t *tid)
{
	int i;
	arr[counter++] = *tid;
	while(1)
	{
		if(b >= 24)
			break;
		pthread_mutex_lock(&lock); 
		printf("\nThread ID - %u, Process ID - %d",*tid,getpid());
		b++;
		for(i=0;i<3;i++)
		{
			if(arr[i]==*tid)
			{
				count_arr[i]++;
			}
		}
		printf(", Buffer Value Previous: %d, Buffer Value Now: %d\n",b-1,b);
		pthread_mutex_unlock(&lock); 
		sleep(1);
		pthread_barrier_wait(&ba);
	}
}