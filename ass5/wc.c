#include<stdio.h>
#include<string.h>
int main(int argc, char *argv[])
{
	int lines = 1, words = 1, characters = 0;
	char ch;
	//FILE *fp = fopen("test.txt","r"); 
	if(strcmp(argv[1],"-l")==0)
	{
		while ((ch = getchar() )!= EOF)
		{
			if (ch == '\n' || ch == '\0')
			{
			 	lines++;
			}
			
		}

		if(characters > 0)
		{
			lines++;
		}

		printf("\n");
		printf("Total lines      = %d\n", lines);	
	}
	else if(strcmp(argv[1],"-c")==0)
	{
		while ((ch = getchar() )!= EOF)
		{
			if(ch != ' ' && ch != '\t' && ch != '\n' && ch != '\0')
				characters++;
		}

		printf("\n");
		printf("Total characters = %d\n", characters);
	}
	else if(strcmp(argv[1],"-w")==0)
	{
		while ((ch = getchar() )!= EOF)
		{
			if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\0')
			{
			    words++;
			}
		}

		if(characters > 0)
		{
			words++;
		}

		printf("\n");
		printf("Total words      = %d\n", words);	
	}
	
	//fclose(fp);
	return 0;
}
