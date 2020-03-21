#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/types.h>


#define clear() printf("\033[H\033[J")


int mycd(char **);
int myexit(char **);

int pipelen = 0;
char *mycmd[] = {
  "cd",
  "exit"
};

int (*intcmd[]) (char **) = {
  &mycd,
  &myexit
};




int my_cmd_len(void);
void infloop(void);
int execute(char *[][100]);
int extcmd(char **);
char *read_line(void);
int semicolontokenizer(char*, char**);
int pipetokenizer(char* , char**);
void spacetokenizer(char* , char**);
int tokenizer(char *,char **,char **, char *[][100]);
int split_line(char *);
void printDir(void);
void setup(void);
char* myspaceremover(char *, char *);







int main(int argc, char **argv)
{

  setup(); 


  infloop();

  return 0;
}

/*FUNC DEF*/


void setup()
{
	clear();
	printf("---------------------------MYSHELL--------------------------\n");
	char* user = "Mayukh";
	printf("Owner  = %s : \n",user);
	//sleep(2);clear();
}


void printDir() 
{ 
	char cwd[1024]; 
	getcwd(cwd, sizeof(cwd)); 
	printf("%s", cwd); 
	//return cwd;
}

int my_cmd_len() {
  return sizeof(mycmd) / sizeof(char *);
}

int mycd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "mycd: expected argument: \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("mycd");
    }
  }
  return 1;
}


int myexit(char **args)
{
  return -1;
}

void infloop(void)
{
  char *line;
  int flag;

  do {
		printf("\033[1;31m");
    printf("\nMyShell@");
		printf("\033[0;36m");
    printDir();
    printf("$~");
		printf("\033[1;10m");

    line = read_line();
    flag = split_line(line);
    free(line);
  } while (flag);
}

int execute(char *cmd[][100])
{
			int i;
			if(fork() == 0)
			{
				int pfd[pipelen-1][2];
				if(pipe(pfd[0]) < 0)
				{
					printf("Error during pipe creation\n");
					exit(0);
				}
				if(fork() == 0)
				{
						for(i=1;i<pipelen-1;i++)
						{
								if(pipe(pfd[i]) < 0)
								{
									printf("Error during pipe creation\n");
									exit(0);
								}
								if(fork() != 0)
								{
										wait(NULL);
										//read end
										close(pfd[i][1]);
										dup2(pfd[i][0],0);
										close(pfd[i][0]);
										//write end
										close(pfd[i-1][0]);
										dup2(pfd[i-1][1],1);
										close(pfd[i-1][1]);
										execvp(cmd[pipelen-i-1][0],cmd[pipelen-i-1]);
								}
						}
						close(pfd[i-1][0]);
						dup2(pfd[i-1][1],1);
						close(pfd[i-1][1]);
						execvp(cmd[pipelen-i-1][0],cmd[pipelen-i-1]);
						
				}
				else
				{
						wait(NULL);
						close(pfd[0][1]);
						dup2(pfd[0][0],0);
						close(pfd[0][0]);
						execvp(cmd[pipelen-1][0],cmd[pipelen-1]);
				}
			}
			else{
				wait(NULL);
			}


	return 1;
}


int extcmd(char **args)
{
  pid_t pid;
  int flag;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0],args) == -1) {
      perror("\nexecvp failure!!\n");
    }
    exit(0);
  } else if (pid < 0) {
    perror("\nfork failure!!\n");
  } else {
    do {
      waitpid(pid, &flag, WUNTRACED);
    } while (!WIFEXITED(flag) && !WIFSIGNALED(flag));
  }

  return 1;
}

int extcmd_ver2(char **args)
{
  pid_t pid;
  int flag;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      printf("\nexecvp failure!!\nNo such commands\n");
      return -1;
    }
    exit(0);
  } else if (pid < 0) {
    perror("\nfork failure!!\n");
  } else {
    do {
      waitpid(pid, &flag, WUNTRACED);
    } while (!WIFEXITED(flag) && !WIFSIGNALED(flag));
  }

  return 1;
}

char *read_line(void)
{
  int pos = 0;
  char *buffer = malloc(sizeof(char) * 1024);
  char ch;

  if (!buffer) {
    fprintf(stderr, "\nAllocation error!!\n");
    exit(0);
  }

  while (1) {
    ch = getchar();

    if (ch == EOF) 
    {
      exit(EXIT_SUCCESS);
    } 
    else if (ch == '\n') 
    {
      buffer[pos] = '\0';
      return buffer;
    } 
    else 
    {
      buffer[pos] = ch;
    }
    pos++;
  }

  /*while((ch = getchar())!=EOF)
	{
		msg[i] = ch;
		i++;
	}*/
}


int semicolontokenizer(char* line, char** semicolontoken)
{
	int i;
	for(i=0; i<1024; i++){
		semicolontoken[i]=strsep(&line, ";");
		if(semicolontoken[i]==NULL)
			break;
	}	
	return i;
} 

int pipetokenizer(char* str, char** strpiped) 
{ 
	int i; 
	for (i = 0; i < 1024; i++) { 
		strpiped[i] = strsep(&str, "|"); 
		if (strpiped[i] == NULL) 
			break; 
	} 
	//printf("\t%s\t%s\t",strpiped[0],strpiped[1]);
	if (strpiped[1] == NULL) 
		return 0; 
	else 
		return i; 
} 

char* myspaceremover(char *str, char *parsed)
{
	int i = 0, j = 0;
	char minimal[1024];
	while (str[i] != '\0')
    {
      if (str[i] == ' ' && str[i+1] == '-') 
	  {
        minimal[j] = str[i];
        j++;
      }
	  else if(str[i] != ' ')
	  {
		minimal[j] = str[i];
        j++;
	  }
      i++;
    }
 
     minimal[j] = '\0';
	 parsed = minimal;
	 return parsed;
}

void spacetokenizer(char* str, char** parsed)
{ 
	int i; 
	for (i = 0; i < 1024; i++) 
	{ 
		parsed[i] = strsep(&str, " "); 
		if (parsed[i] == NULL) 
			break; 
		if (strlen(parsed[i]) == 0) 
			i--; 
	} 
} 

int tokenizer(char *semicolontoken,char **token_norm,char **token_piped, char *token[][100]) 
{ 
	int i;
	char* strpiped[1024]; 
	int piped = 0,flag = 0, f = 0; 
	char *andp = strstr(semicolontoken,"&&");
	char *orp = strstr(semicolontoken,"||");
	if(orp)
	{
		for (i = 0; i < 3; i++){
			strpiped[i] = strsep(&semicolontoken, "||"); 
			if (strpiped[i] == NULL) 
				break; 
		}
		spacetokenizer(strpiped[0], token_norm); 
		spacetokenizer(strpiped[2], token_piped);
		piped = 2;

	}
	else if(andp)
	{
		for (i = 0; i < 3; i++)
		{
			strpiped[i] = strsep(&semicolontoken, "&&"); 
			if (strpiped[i] == NULL) 
				break; 
		}
		spacetokenizer(strpiped[0], token_norm); 
		spacetokenizer(strpiped[2], token_piped);
		//printf("\t%s\t%s\t",strpiped[0],strpiped[1]);
		piped = 3;
	}
	else
	{
		piped = pipetokenizer(semicolontoken, strpiped); 

		if(piped)
		{
			pipelen = piped;
			piped = 1;
		}
		if(piped){ 
			//spacetokenizer(strpiped[0], token_norm); 
			//spacetokenizer(strpiped[1], token_piped);


			for(i=0;i<pipelen;i++)
			{
				spacetokenizer(strpiped[i], token[i]);
			}

			f = 1;
		} 
		else 	
			spacetokenizer(semicolontoken, token_norm); 
	}
	
	if(!f)
	{
		for (i = 0; i < my_cmd_len(); i++) 
		{
				if (strcmp(token_norm[0], mycmd[i]) == 0) 
				{
						flag = (*intcmd[i])(token_norm);
						if(flag == -1)
							return -1;
						return 0;
				}
			}
	}
	return 1 + piped; 
}

int split_line(char *line)
{
	int execFlag;
	int i;
  	char* semicolontoken[1024];
	int token_len = semicolontokenizer(line, semicolontoken);
	for(i=0;i<token_len;i++){


		char *token[100][100];

		char* token_norm[1024]; 
		
		char* token_piped[1024]; 
		
		execFlag = 0; 
		
		execFlag = tokenizer(semicolontoken[i], token_norm, token_piped, token); 
		if (execFlag == 1) 
			extcmd(token_norm); 
		if (execFlag == 2) 
			execute(token);
		if (execFlag == 3) 
		{
			if((extcmd_ver2(token_norm)) == -1)
			{
				extcmd(token_piped);
			}
		}
		if (execFlag == 4) 
		{
			if((extcmd_ver2(token_norm)) == -1)
			{
				return 1;
			} 
			extcmd(token_piped);
		}

	}
	if(execFlag == -1)
		return 0;
	return 1;
}