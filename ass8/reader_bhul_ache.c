#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<errno.h>
#include<string.h>

union semun
{
	int val;
	struct semid_ds *buf;
	ushort* array;
}arg;

struct data
{
	int readcount;
};
struct data *p;
 
void wait(int semid,int semnum)
{
	printf("\nIn Wait\n");
	struct sembuf a;
	a.sem_num=semnum;
	a.sem_op=-1;
	a.sem_flg=0;
	semop(semid,&a,1);
}

void signal(int semid,int semnum)
{
	printf("\nIn Signal\n");
	struct sembuf a;
	a.sem_num=semnum;
	a.sem_op=1;
	a.sem_flg=0;
	semop(semid,&a,1);
}

int main(int argc,char* argv[])
{
	pid_t pid;
	key_t key1, key2;
	int sem_id, shm_id, fd, no, i;
	char buf[100];
	
	key1=ftok("./",4567);
	shm_id=shmget(key1,sizeof(struct data *),IPC_CREAT | 0666);
	printf("SHMID -:> %d\n",shm_id);
	if(shm_id==-1)
		printf("%d\n",errno);



	key2=ftok("./",1234);
	sem_id = semget(key2, 3, IPC_CREAT | IPC_EXCL | 0666);
	int wrt = 0, mutex = 1;
	p = (struct data *)shmat(shm_id, NULL, 0);
	if((int)p<0)
	{
		printf("attaching isn't possible for shmat()\n");
		semctl(sem_id,0,IPC_RMID,0);
		exit(0);
	}
	if(sem_id!=-1)
	{
		printf("SEMID -:> %d\n",sem_id);
		p->readcount = 0;
		//printf("\nhello\n");
		semctl(sem_id,wrt,SETVAL,1);
		//printf("\nhi\n");
		semctl(sem_id,mutex,SETVAL,1);
		//printf("\nbye\n");
	}

	while(1)
	{
		printf("\nEnter 0 to quit else continue: ");
		scanf("%d",&no);
		if(no==0)
			break;
		wait(sem_id,mutex);
		p->readcount = p->readcount + 1;
		if(p->readcount == 1)
			wait(sem_id,wrt);
		signal(sem_id,mutex);

		printf("\nWRT - %d\n",semctl(sem_id,wrt,GETVAL));
		printf("\nMUTEX - %d\n",semctl(sem_id,mutex,GETVAL));
		printf("\nReading Started...\n");
		fd=open("foo.txt", O_RDONLY);
		if(read(fd, buf, 100) > 0)
		{
			buf[99] = '\0';
			i = 0;
			while(buf[i]!='\0')
			{
				putchar(buf[i]);
				i++;
			}
		}
		printf("\n");
		if(close(fd) < 0){
			printf("Error while closing.\n");
			exit(0);
		}
		sleep(10);
		printf("\nReading Completed...\n");


		wait(sem_id, mutex);
		p->readcount = p->readcount - 1;
		if(p->readcount == 0)
			signal(sem_id, wrt);
		signal(sem_id,mutex);
	}	

	shmdt((void *)p);
	shmctl(shm_id,IPC_RMID,NULL);
	semctl(sem_id,0,IPC_RMID,0);
	return 0;
}


