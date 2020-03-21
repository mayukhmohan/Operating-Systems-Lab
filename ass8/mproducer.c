#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<errno.h>
#include<string.h>

#define n 10
struct data
{
	int circularQueue[10];
	int in,out,sflag,cflag;
};

union semun
{
	int val;
	struct semid_ds *buf;
	ushort* array;
}arg;
struct data *p;

/*
struct semid_ds {
. . .
struct sem *sem_base; /*pointer to first semaphore in set
ushort_t sem_nsems; /* number of semaphores in set 
};


struct sem {
ushort semval; /*semaphore value
pid_t sempid; /*pid of last operation 
ushort semcnt; /* # waiting for increase in semval 
ushort semzcnt; /* # waiting for semval == 0 
};


struct sembuf {
ushort sem_num; /* semaphore # 
short sem_op; /*semaphore operation
short sem_flg; /*operation flags
};

*/

void wait(int semid,int semnum)
{
	struct sembuf a;
	a.sem_num=semnum;
	a.sem_op=-1;
	a.sem_flg=0;
	semop(semid,&a,1);
}

void signal(int semid,int semnum)
{
	struct sembuf a;
	a.sem_num=semnum;
	a.sem_op=1;
	a.sem_flg=0;
	semop(semid,&a,1);
}

int main(int argc,char* argv[])
{
	srand(time(0));
	pid_t pid;
	key_t key1,key2;
	key1=ftok("./",1234);
	int shm_id, sem_id, item;

	shm_id=shmget(key1,sizeof(struct data),IPC_CREAT | 0666);
	printf("SHMID -:> %d\n",shm_id);

	if(shm_id==-1)
		printf("%d\n",errno);

	key2=ftok("./",4567);
	sem_id = semget(key2, 3, IPC_CREAT|0666);
	printf("SEMID -:> %d\n",sem_id);

	if(sem_id==-1)
		printf("%d\n",errno);

	p = (struct data *)shmat(shm_id, NULL, 0);
	if((int)p<0)
	{
		printf("attaching isn't possible for shmat()\n");
		exit(0);
	}

	//semop (sem_id, struct sembuf *sops, size_t nsops);
	int empty=0,full=1,mutex=2,no;
	if(p->sflag != 1)
	{
		p->in = 0;
		semctl(sem_id,empty,SETVAL,n);
		semctl(sem_id,full,SETVAL,0);
		semctl(sem_id,mutex,SETVAL,1);
	}
	p->sflag = 1;

	while(1)
	{	
		/*printf("Enter 0 to quit else continue: ");
		scanf("%d",&no);
		if(no==0)
			break;*/
		wait(sem_id,empty);
		wait(sem_id,mutex);
		sleep(1);

		item = rand()%100+1;

		p->in = (p->in + 1)%n;
		p->circularQueue[p->in] = item;

		printf("\nProducer: %d:-> produced %d\n",getpid(),item);

		signal(sem_id,mutex);
		signal(sem_id,full);
	}



	shmdt((void *)p);
	shmctl(shm_id,IPC_RMID,NULL);
	semctl(sem_id,0,IPC_RMID,0);
	return 0;
}
