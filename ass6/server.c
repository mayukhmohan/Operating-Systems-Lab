#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>

struct data
{
	long long int data;
	int cdone,sdone,final;//ready;
	int term;
};
struct data *p;

long long int fact(long long int n)
{
	if(n==0 || n==1)
		return 1;
	return n * fact(n-1);
}

int main(int argc,char* argv[])
{
	key_t key;
	key=ftok("./",1234);
	int shm_id;

	shm_id=shmget(key,sizeof(struct data),IPC_CREAT | 0666);
	printf("SHMID -:> %d\n",shm_id);

	if(shm_id==-1)
		printf("%d\n",errno);


	p = (struct data *)shmat(shm_id, NULL, 0);
	if((int)p<0)
	{
		printf("attaching isn't possible for shmat()\n");
		exit(0);
	}

	int i = 0;
	p->term = 1;
	p->sdone = 0;
	while(p->term)
	{
		//p->ready = 1;
		p->sdone = 0;
		sleep(1);
		if(p->cdone)
		{
			printf("*****Data Received*****\n");
			printf("$-> %lld factorial is %lld\n",p->data,fact(p->data));
			p->sdone = 1;
			i++;
			if(i == 20)
				p->term = 0;
		}
	}
	p->final = 1;
	shmdt((void *)p);
	shmctl(shm_id,IPC_RMID,NULL);
	return 0;
}
