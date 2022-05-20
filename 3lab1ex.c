#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

void childProc()
{
	struct timeval *tv;
	tv = (struct timeval*)malloc(sizeof(struct timeval));
	
	fprintf(stdout, "Procces with pid=%d, and with ppid=%d\n", getpid(), getppid());
	
	if(gettimeofday(tv, NULL) == 0)
	{
		long int h = (tv->tv_sec/(60*60) + 3)% 24;
		long int m = (tv->tv_sec/60) % 60;
		long int s = (tv->tv_sec) % 60;
		long int ms = tv->tv_usec/1000;
		fprintf(stdout, "%ld:%ld:%ld:%ld\n", h, m, s, ms);
	}
	else
	{
		perror("gettimeofday:");
		exit(EXIT_FAILURE);
	}
	return;
}

void main()
{
	pid_t p;
	
	p = fork();
	if(p==-1)
	{
		perror("fork:");
	}
	else if(p==0)
	{
		fprintf(stdout, "Child process N1:\n");
		childProc();
	}
	else
	{
		p = fork();
		if(p==-1)
		{
			perror("fork:");
		}
		else if(p==0)
		{
			fprintf(stdout, "Child process N2:\n");
			childProc();
		}
		else
		{
			fprintf(stdout, "Parent process:\n");
			childProc();
			
			system("ps -x");
		}
	}
	exit(EXIT_SUCCESS);
}
