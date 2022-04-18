#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define SIG_OK 0
#define SIG_NG -1
#define LOOP_ON 0
#define LOOP_OFF 1

int loopFlag = LOOP_ON;

// fuction to handle each signal
void sigHandleSIGALRM();

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Lack of argument!\n");
        return -1;
    }
    int time_clock = atoi(argv[1]);
    alarm(time_clock);
    signal(SIGALRM, sigHandleSIGALRM);

    // infinity loop
    while (loopFlag == LOOP_ON)
    {
    }
}

void sigHandleSIGALRM()
{
    printf("\nHello World!");
    loopFlag = LOOP_OFF;
}