#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_SEND_SIZE 80
#define ESC_CODE 27

const long TYPE_TYPE = 1; //тип сообщения, в котором хранится тип сообщений последнего подключенного клиента
const long MESSAGE_TYPE = 2; //тип сообщений, которые считывает сервер для рассылки всем подключенным клиентам
const long INITIAL_TYPE = 3; //тип первого подключенного клиента

struct mymsgbuf {
	long mtype;
	char mtext[MAX_SEND_SIZE];
};

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text) 
{
	printf("Send message %s\n", text);
	qbuf->mtype = type;
	strcpy(qbuf->mtext, text);

	if ((msgsnd(qid, (struct msgbuf *) qbuf, strlen(qbuf->mtext) + 1, 0)) == -1) 
	{
		perror("msgsnd");
		exit(1);
	}
}

int read_count(int qid, struct mymsgbuf *qbuf, long type, int flag) 
{
	qbuf->mtype = type;
	msgrcv(qid, (struct msgbuf *) qbuf, MAX_SEND_SIZE, type, flag);
	return atoi(qbuf->mtext);
}

char* read_message(int qid, struct mymsgbuf *qbuf, long type, int flag) 
{
	qbuf->mtype = type;
	msgrcv(qid, (struct msgbuf *) qbuf, MAX_SEND_SIZE, type, flag);
	return qbuf->mtext;
}

void remove_queue(int qid) 
{
	/* Remove the queue */
	msgctl(qid, IPC_RMID, 0);
}

int main(int argc, char *argv[]) 
{
	key_t key;
	int msgqueue_id;
	struct mymsgbuf qbuf;
	struct mymsgbuf outbuf;
	{
		
	};
	
	/* Create unique key via call to ftok() */
	key = ftok(".", 'm');

	if ((msgqueue_id = msgget(key, IPC_CREAT | 0660)) == -1) 
	{
		perror("msgget");
		exit(1);
	}

	char temp[2] = "3";
	send_message(msgqueue_id, (struct mymsgbuf *) &qbuf, TYPE_TYPE, temp);

  	int c = 0;
  	pid_t pid = fork();

  	while (c != ESC_CODE)
  	{
  		if (pid == 0)
  		{
	  		printf("Waiting messages...\n");
	  		char *msg = read_message(msgqueue_id, &outbuf, MESSAGE_TYPE, 0);

	  		int client_last_type = read_count(msgqueue_id, &qbuf, TYPE_TYPE, 0);
	  		sprintf(temp, "%d", client_last_type);
	  		send_message(msgqueue_id, (struct mymsgbuf *) &qbuf, TYPE_TYPE, temp);

	  		int i;
			for (i = INITIAL_TYPE; i < client_last_type; i++)
				send_message(msgqueue_id, (struct mymsgbuf *) &outbuf, i, msg);
		}
		else c = getchar();
	}

	printf("\n");

	return (0);
}

