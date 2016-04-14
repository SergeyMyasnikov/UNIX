#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_SEND_SIZE 30
#define ESC_CODE 27
#define RETURN_CODE 10

const long TYPE_TYPE = 1; //тип сообщения, в котором хранится тип сообщений последнего подключенного клиента
const long MESSAGE_TYPE = 2; //тип сообщений, которые считывает сервер для рассылки всем подключенным клиентам
int client_type;

struct mymsgbuf {
	long mtype;
	char mtext[MAX_SEND_SIZE];
};

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text)
{
	qbuf->mtype = type;
	strcpy(qbuf->mtext, text);
	printf("%d-client send message: %s\n", client_type, text);

	if ((msgsnd(qid, (struct msgbuf *) qbuf, strlen(qbuf->mtext) + 1, 0)) == -1) 
	{
		perror("msgsnd");
		exit(1);
	}
}

int read_type(int qid, struct mymsgbuf *qbuf, long type, int flag)
{
	qbuf->mtype = type;
	msgrcv(qid, (struct msgbuf *) qbuf, MAX_SEND_SIZE, type, flag);
	return atoi(qbuf->mtext);
}

void read_message(int qid, struct mymsgbuf *qbuf, long type, int flag) 
{
	qbuf->mtype = type;
	msgrcv(qid, (struct msgbuf *) qbuf, MAX_SEND_SIZE, type, flag);
	printf("%d-client recieve: %s\n", client_type, qbuf->mtext);
}

void remove_queue(int qid) 
{
	msgctl(qid, IPC_RMID, 0);
}

int main(int argc, char *argv[]) 
{
	key_t key;
	int msgqueue_id;
	struct mymsgbuf qbuf;

	/* Create unique key via call to ftok() */
	key = ftok(".", 'm');

	if ((msgqueue_id = msgget(key, IPC_CREAT | 0660)) == -1) 
	{
		perror("msgget");
		exit(1);
	}

  	int c = 0;
  	int i = 0;
  	client_type = read_type(msgqueue_id, &qbuf, TYPE_TYPE, 0);
  	char temp[2];
  	sprintf(temp, "%d", client_type + 1);
  	send_message(msgqueue_id, (struct mymsgbuf *) &qbuf, TYPE_TYPE, temp);

  	char buf[MAX_SEND_SIZE];
  	pid_t pid = fork();

	while (c != ESC_CODE) 
	{
		if (pid == 0)
		{
			struct mymsgbuf inpbuf;
			read_message(msgqueue_id, &inpbuf, client_type, 0);
		}
		else
		{
			c = getchar();
			if (i < MAX_SEND_SIZE) 
			{
		    	buf[i++] = c;
			} 
			else 
			{
	      		printf("Very big message!\n");
	      		i = 0;
	      		continue;
			}

			if (c == RETURN_CODE) 
			{
				if (i > 1) 
				{
			    	buf[i] = 0;
			    	struct mymsgbuf outbuf;
					send_message(msgqueue_id, (struct mymsgbuf *) &outbuf, MESSAGE_TYPE, buf);
			  	}
				i = 0;
			}
			
		}
	}
	
	printf("\n");

	return (0);
}

