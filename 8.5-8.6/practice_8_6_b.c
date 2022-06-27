#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

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

    printf("processB(MessageSend):   start\n");
    keyx = ftok(KEYFILE_PATH, (int)ID);

    // Message queue ID acquisition
    if((msqid = msgget(keyx, IPC_CREAT | 0660)) == MSGQ_NG)
    {
        perror("msgget: processB(MessageSend)");
        exit(1);
    }
    printf("processB(MessageSend): msqID = %d\n",msqid);

    // Send message:
    message.mtype = 1;
    strcpy(message.mtext,"Hello, hello! My name's");

    // Send to message queue with ID msqid: Message buffer structure, message type 1.
    if(msgsnd(msqid, &message, sizeof(message.mtext), 0) == MSGQ_NG)
    {
        perror("msgrcv: processB(MessageSend)");
        exit(1);
    }
    printf("processB(MessageSend): sent message = %s\n",message.mtext);

    printf("processB(MessageSend): end\n");
}