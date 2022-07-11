#ifndef _SIGS_H_
#define _SIGS_H_
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>


enum {LIFT_MNG,LIFT_CTR,OPE_PANE1,OPE_PANE2,OPE_PANE3,OPE_PANE4,OPE_PANE5,LIFT_POSITION};
#define CLOCK 300000
#define TRUE 1
#define FALSE 0
#define SUCCESS 1
#define FAILED 0

#define UP 1
#define STAND 0
#define DOWN -1

#define SHARE_KEY 2468

#define F1_CALL 11
#define F2_CALL 12
#define F3_CALL 13
#define F4_CALL 14
#define F5_CALL 15
#define F1_UP 16
#define F2_UP 17
#define F3_UP 18
#define F4_UP 19
#define F5_UP 20

#define USING 9
#define FINISHED 10
#define FINISHEDUSING 21

#define LIFT_UP 6
#define LIFT_DOWN 7
#define LIFT_STOP 8


#define F1_ARRIVAL 1
#define F2_ARRIVAL 2
#define F3_ARRIVAL 3
#define F4_ARRIVAL 4
#define F5_ARRIVAL 5

#define WAIT_TIME 3
pid_t* update_pid(int i);
void release_shm();
int send_signal(pid_t pid,int sigNo);
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/fcntl.h>

#define FIFO_FILE_PATH "requestsqueue"
#define OPE1_FIFO_FILE "ope1fifofile"
#define BUFF_SIZE 256
#endif
