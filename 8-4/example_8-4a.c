#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEYFILE_PATH "keyfilepath"
#define ID 'S'
#define SEM_OK 0
#define SEM_NG -1

int main(void)
{
    int semid;
    int semval;
    key_t keyval;
    struct sembuf buff;

    printf("processA    :start\n");

    keyval = ftok(KEYFILE_PATH, (int)ID);

    semid = semget(keyval, 1, IPC_CREAT|0660);

    printf("processA(sem-): semID = %d\n",semid);

    buff.sem_num = (ushort)0;
    buff.sem_op = (short)-4;
    buff.sem_flg = (short)0;

    semop(semid, &buff, 1);
    printf("processA(sem-): sem_op = %d\n", buff.sem_op);

    semval = semctl(semid, 0, GETVAL, 0);
    printf    

}