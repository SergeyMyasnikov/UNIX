#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

int read_file(int *matrix, int *vector)
{
	FILE *fp = fopen("input.txt", "r");

	int n;
	fscanf(fp, "%d", &n);

	matrix = (int*)malloc(sizeof(int) * n * n);
	vector = (int*)malloc(sizeof(int) * n);

	int i, j;

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
			fscanf(fp, "%d", &matrix[i * n + j]);
		fscanf(fp, "\n");
	}
	
	fscanf(fp, "\n");

	for (i = 0; i < n; i++)
		fscanf(fp, "%d", &vector[i]);
	
	fclose(fp);
	
	return n;
}

void print_data(int *data, int start_row, int end_row, int size)
{
	int i, j;
	for (i = start_row; i < end_row; i++)
	{
		for (j = 0; j < size; j++)
			printf("%d ", data[i * size + j]);
		printf("\n");
	}
	printf("\n");
}

void writeshm(int shmid, int *segptr, int *data, int start_row, int end_row, int size) 
{
	int i, j;
	for (i = start_row; i < end_row; i++)
		for (j = 0; j < size; j++)
			segptr[i * size + j] = data[(i - start_row) * size + j];
}

void readshm(int shmid, int *segptr, int start_row, int end_row, int size) 
{
	FILE *f;
	f = fopen("output.txt", "w");
	int i, j;
	for (i = start_row; i < end_row; i++)
	{
		for (j = 0; j < size; j++)
			fprintf(f, "%d ", segptr[i * size + j]);
		fprintf(f, "\n");
	}
	fclose(f);
}

void removeshm(int shmid) 
{
	shmctl(shmid, IPC_RMID, 0);
	printf("Shared memory segment marked for deletion\n");
}

void mult(int shmid, int *segptr, int start_row, int end_row, int size)
{
	int i, j;
	for (i = start_row; i < end_row; i++)
	{
		segptr[(size + 1) * size  + i] = 0;
		for (j = 0; j < size; j++)
		{
			segptr[(size + 1) * size  + i] += segptr[i * size + j] * segptr[size * size + j];
		}
	}
}

int main(int argc, char *argv[])
{
	int *matrix;
	int *vector;

	FILE *fp = fopen("input.txt", "r");

	int n;
	fscanf(fp, "%d", &n);

	matrix = (int*)malloc(sizeof(int) * (n + 1) * (n + 1));
	vector = (int*)malloc(sizeof(int) * (n + 1));

	int i, j;

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
			fscanf(fp, "%d", &matrix[i * n + j]);
		fscanf(fp, "\n");
	}
	
	fscanf(fp, "\n");

	for (i = 0; i < n; i++)
		fscanf(fp, "%d", &vector[i]);
	
	fclose(fp);

	key_t key;
	int shmid;
	int *segptr;

	size_t SEGSIZE = sizeof(int) * (n + 2) * (n + 2);

	key = ftok(".", 'S');

	if ((shmid = shmget(key, SEGSIZE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
	{
		printf("Shared memory segment exists - opening as client\n");
		if ((shmid = shmget(key, SEGSIZE, 0)) == -1) 
		{
			perror("shmget");
			exit(1);
		}
	} 
	else printf("Creating new shared memory segment\n");
	
	if ((segptr = shmat(shmid, 0, 0)) == (int *)-1) 
	{
		perror("shmat");
		exit(1);
	}

	writeshm(shmid, segptr, matrix, 0, n, n);
	writeshm(shmid, segptr, vector, n, n + 1, n);

	int thread_count = atoi(argv[1]);
	int rows_on_thread[thread_count];
	int thread_pid[thread_count];

	for (i = 0; i < thread_count; i++)
		rows_on_thread[i] = n / thread_count;

	int ost = n % thread_count;
	for (i = 0; i < ost; i++)
	{
		rows_on_thread[i]++;
	}

	int start_row = 0;
	int end_row = 0;
	pid_t pid;

	for (i = 0; i < thread_count; i++)
	{
		start_row = end_row;
		end_row += rows_on_thread[i];	
		pid = fork();
		if (pid == 0)
		{
			mult(shmid, segptr, start_row, end_row, n);
			exit(0);
		}
		else thread_pid[i] = pid;
	}

	if (pid != 0)
	{
		int status;
		for (i = 0; i < thread_count; i++)
		{
			waitpid(thread_pid[i], &status, 0);
			if (WIFEXITED(status)) 
                printf("exited, status=%d\n", WEXITSTATUS(status));
		}
		readshm(shmid, segptr, n + 1, n + 2, n);
		removeshm(shmid);
	}
}