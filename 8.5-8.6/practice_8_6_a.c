#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGSIZE 100
#define KEYFILE_PATH "keyfile"
#define ID 'M'
#define MSGQ_OK 0
#define MSGQ_NG -1

int main()
{
    struct msgbuff {
        long mtype;
        char mtext[MSGSIZE];
    } message;

    int msqid;
    key_t keyx;
    struct msqid_ds msq;

    printf("processA(MessageReceive):   start\n");
    keyx = ftok(KEYFILE_PATH, (int)ID);

    // Message queue ID acquisition
    if((msqid = msgget(keyx, IPC_CREAT | 0660)) == MSGQ_NG)
    {
        perror("msgget: processA(MessageReceive)");
        exit(1);
    }
    printf("processA(MessageReceive): msqID = %d\n",msqid);

    // Receive message:
    // Receive from queue ID msqid, save into message buffer: Message of type 1. Block while waiting for sender
    if(msgrcv(msqid, &message, sizeof(message.mtext), 1, 0) == MSGQ_NG)
    {
        perror("msgrcv: processA(MessageReceive)");
        exit(1);
    }
    printf("processA(MessageReceive): received message = %s\n",message.mtext);

    // Delete message queue
    if(msgctl(msqid, IPC_RMID, NULL) == MSGQ_NG)
    {
        perror("msgctl: processA(MessageReceive)");
    }
    printf("processA(MessageReceive): end\n");
}