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
	int m,n;
	int a[100][100],b[100][100];
	int diff;
};
struct data *p;

int main(int argc,char* argv[])
{
	pid_t pid;
	key_t key;
	key=ftok("./",1234);
	int shm_id;
	srand(time(0));

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

	printf("Enter row number:");
	scanf("%d",&p->m);
	printf("Enter column number:");	
	scanf("%d",&p->n);
	printf("Enter Difference value:");
	scanf("%d",&p->diff);

	int i = 0, j = 0,k,l, temp = 0,max;

	for(i=0;i<p->m+2;i++)
	{
		for(j=0;j<p->n+2;j++)
		{
			if(i==0 || j==0 || i==p->m+1 || j==p->n+1)
			{
				p->a[i][j] = 0;
				p->b[i][j] = 0;
			}
			else
			{
				p->a[i][j] = rand()%100+1;
				p->b[i][j] = 0;
			}
		}
	}

	int turn = 0;
	while(1){
		if(turn == 0)
		{
			for(i=1;i<p->m+1;i++)
			{
				for(j=1;j<p->n+1;j++)
				{
					
					pid = fork();
					if(pid == 0)
					{
						if(i!= 1 && i!= p->m && j!=1 && j!=p->n)
						{
							p->b[i][j] = (p->a[i-1][j-1] + p->a[i-1][j] + p->a[i-1][j+1] + p->a[i][j-1] + p->a[i][j] + p->a[i][j+1] + p->a[i+1][j-1] + p->a[i+1][j] + p->a[i+1][j+1])/9;
						}
						else if((i==1 && j==1) || (i==1 && j==p->n) || (i==p->m && j==1) || (i==p->m && j==p->n))
						{
							p->b[i][j] = (p->a[i-1][j-1] + p->a[i-1][j] + p->a[i-1][j+1] + p->a[i][j-1] + p->a[i][j] + p->a[i][j+1] + p->a[i+1][j-1] + p->a[i+1][j] + p->a[i+1][j+1])/4;
						}
						else
						{
							p->b[i][j] = (p->a[i-1][j-1] + p->a[i-1][j] + p->a[i-1][j+1] + p->a[i][j-1] + p->a[i][j] + p->a[i][j+1] + p->a[i+1][j-1] + p->a[i+1][j] + p->a[i+1][j+1])/6;
						}
						printf("child: %d;PID: %d\n",i+j-1,getpid());
						printf("Average of A[i][j] = %d is B[i][j] = %d\n",p->a[i][j],p->b[i][j]);
						shmdt((void *)p);
						exit(0);				
					}
					else
					{
						wait(NULL);
					}
				}
			}
		}
		else
		{
			for(i=1;i<p->m+1;i++)
			{
				for(j=1;j<p->n+1;j++)
				{
					
					pid = fork();
					if(pid == 0)
					{
						if(i!= 1 && i!= p->m && j!=1 && j!=p->n)
						{
							p->b[i][j] = (p->b[i-1][j-1] + p->b[i-1][j] + p->b[i-1][j+1] + p->b[i][j-1] + p->b[i][j] + p->b[i][j+1] + p->b[i+1][j-1] + p->b[i+1][j] + p->b[i+1][j+1])/9;
						}
						else if((i==1 && j==1) || (i==1 && j==p->n) || (i==p->m && j==1) || (i==p->m && j==p->n))
						{
							p->b[i][j] = (p->b[i-1][j-1] + p->b[i-1][j] + p->b[i-1][j+1] + p->b[i][j-1] + p->b[i][j] + p->b[i][j+1] + p->b[i+1][j-1] + p->b[i+1][j] + p->b[i+1][j+1])/4;
						}
						else
						{
							p->b[i][j] = (p->b[i-1][j-1] + p->b[i-1][j] + p->b[i-1][j+1] + p->b[i][j-1] + p->b[i][j] + p->b[i][j+1] + p->b[i+1][j-1] + p->b[i+1][j] + p->b[i+1][j+1])/6;
						}
						printf("child: %d;PID: %d\n",i+j-1,getpid());
						printf("Average of A[i][j] = %d is B[i][j] = %d\n",p->a[i][j],p->b[i][j]);
						shmdt((void *)p);
						exit(0);				
					}
					else
					{
						wait(NULL);
					}
				}
			}
		}
		if(turn == 0)
			turn = 1;
		max = -999999;
		for(i=1;i<p->m+1;i++)
		{
			for(j=1;j<p->n+1;j++)
			{
				for(k = i-1;k<=i+1;k++)
				{
					for(l = j-1;l<=j+1;l++)
					{
						if(p->b[k][l] == 0)
							continue;
						if(i==k && j==l)
							continue;
						else
						{
							temp = p->b[i][j] - p->b[k][l];
							if(temp<0)
								temp = -temp;
							if(max<temp)
								max = temp;
						}
					}
				}
			}
		}
		if(max<=p->diff)
			break;
		for(i=0;i<p->m+2;i++)
		{
			for(j=0;j<p->n+2;j++)
			{
				printf("%15d",p->b[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	}

	for(i=0;i<p->m+2;i++)
	{
		for(j=0;j<p->n+2;j++)
		{
			printf("%15d",p->a[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for(i=0;i<p->m+2;i++)
	{
		for(j=0;j<p->n+2;j++)
		{
			printf("%15d",p->b[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	shmdt((void *)p);
	shmctl(shm_id,IPC_RMID,NULL);
	return 0;
}
