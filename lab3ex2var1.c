#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define FILE_NAME "./file.txt"

float func(int i, int N, int j)
{
	float p = 1;
	float x = 2 * 3.14 * (float)i / (float)N;
	float nx = 1;
	for(int k=1; k<=2*j-1; k++)
	{
		nx *= x;
		p *= -k;
	}
	return nx / p;
}

void main(int argc, char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Need 2 parametrs! y[i] = (2 * PI * i / N). 1st parametr - amount of Teylor sequence, 2nd - N\n");
		exit(EXIT_FAILURE);
	}

	errno = 0;
	char *str, *endptr;
	str = argv[1];
	long val = strtol(str, &endptr, 0);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
	{
		perror("strtol");
		exit(EXIT_FAILURE);
	}
	if (endptr == str)
	{
		fprintf(stderr, "No digits were found\n");
		exit(EXIT_FAILURE);
	}
	int n = val;
	errno = 0;
	str = argv[2];
	val = strtol(str, &endptr, 0);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
	{
		perror("strtol");
		exit(EXIT_FAILURE);
	}
	if (endptr == str)
	{
		fprintf(stderr, "No digits were found\n");
		exit(EXIT_FAILURE);
	}
	int N = val;
	
	FILE* f = fopen(FILE_NAME, "w");
	if(f == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	if(fclose(f) == EOF)
	{
		perror("fclose");
		exit(EXIT_FAILURE);
	}

	for(int i=0; i<N; i++)
	{
		for(int j=1; j<=n; j++)
		{
			pid_t p = fork();
			if(p==-1)
			{
				perror("fork");
			}
			else if(p==0)
			{
				float tempY = func(i, N, j);
				if(fprintf(stdout, "PID: %d\n%f\n", getpid(), tempY) < 0)
				{
					fprintf(stderr, "fprintf error occured");
					exit(EXIT_FAILURE);
				}
				f = fopen(FILE_NAME, "a");
				if(fprintf(f, "%d\n%f\n", getpid(), tempY) < 0)
				{
					fprintf(stderr, "fprintf error occured");
					exit(EXIT_FAILURE);
				}
				if(fclose(f) == EOF)
				{
					perror("fclose");
					exit(EXIT_FAILURE);
				}
				exit(EXIT_SUCCESS);
			}
			else
			{
				if(wait(NULL) == -1)
				{
					perror("wait");
					exit(EXIT_FAILURE);
				}
				if(j == n)
				{
					f = fopen(FILE_NAME, "r");
					
					float sum = 0;
					
					int t = i*n*2 + i*2;
					char* temp;
					while(t != 0)
					{
						if(fgets(temp, 10000, f) == NULL)
						{
							break;
						}
						t--;
					}
					if(t == 0)
					while(1)
					{
						int tempPid;
						errno = 0;
						if(fscanf(f, "%d", &tempPid) == EOF)
						{
							if(errno != 0)
							{
								perror("fscanf");
								exit(EXIT_FAILURE);
							}
							break;
						}
						
						float tempF;
						errno = 0;
						if(fscanf(f, "%f", &tempF) == EOF)
						{
							if(errno != 0)
							{
								perror("fscanf");
								exit(EXIT_FAILURE);
							}
							break;
						}
						sum += tempF;
					}

					if(fclose(f) == EOF)
					{
						perror("fclose");
						exit(EXIT_FAILURE);
					}
					
					f = fopen(FILE_NAME, "a");
					if(fprintf(stdout, "PID: %d\ny[%d] = %f\n", getpid(), i, sum) < 0)
					{
						fprintf(stderr, "fprintf error occured");
						exit(EXIT_FAILURE);
					}
					if(fprintf(f, "PID: %d\ny[%d] = %f\n", getpid(), i, sum) < 0)
					{
						fprintf(stderr, "fprintf error occured");
						exit(EXIT_FAILURE);
					}

					if(fclose(f) == EOF)
					{
						perror("fclose");
						exit(EXIT_FAILURE);
					}
				}
			}
			
		}
	}
	
	exit(EXIT_SUCCESS);
}
