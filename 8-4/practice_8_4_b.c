#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEYFILE_PATH "keyfile"
#define ID 'S'
#define SEM_OK 0
#define SEM_NG -1

#define UNUSED_ARG 0

union semun 
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

int main()
{
    int semid, semval;
    key_t keyval;
    struct sembuf buff;
    union semun semUnion;

    printf("processB:   start\n");

    // Create semaphore
    keyval = ftok(KEYFILE_PATH, (int)ID);
    semid = semget(keyval, 1, IPC_CREAT | 0660);
    printf("processB(sem): semID = %d\n",semid);

    // Add semaphore by 1
    // EFFECT: Process A (which has been run before) will execute, taking the semaphore
    buff.sem_num = (ushort)0;
    buff.sem_op = (short)1;
    buff.sem_flg = (short)0;
    semop(semid,&buff,1);

    printf("processB    : unblocked processA\n");

    printf("processB    : end\n");
    return 0;
}