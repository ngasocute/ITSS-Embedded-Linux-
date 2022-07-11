#include "sigs.h"
#define MAX_QUEUE 5

pid_t* pid_list;
int lift_is_moving=FALSE;
int fifoFd;
int current_request,current_floor=1;

void up_request(int sigNo){
	//printf("%d %s\n",sigNo,lift_is_moving ? "TRUE" : "FALSE" );
	FILE *f;
	if(lift_is_moving){
		write(fifoFd, &sigNo, sizeof(int));
	}
	else{
		lift_is_moving=send_signal(pid_list[LIFT_CTR],sigNo);
		current_request=sigNo;
	}
}
void send_finish_notification(int sigNo){
	//printf("Mng get ctr: %d, %d\n",sigNo,current_request-SIGRTMIN);
	switch(current_request-SIGRTMIN){
		case F2_UP:	//Nut len tang 2 nhung duoc goi o tang 1
			printf("	Dang chuyen hang o tang 2.\n");
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+FINISHED);	// finished de nut call do -> mau trang
			send_signal(pid_list[OPE_PANE2],SIGRTMIN+USING);		// o tren nut goi chuyen mau xanh
			send_signal(pid_list[OPE_PANE3],SIGRTMIN+F2_ARRIVAL);
			send_signal(pid_list[OPE_PANE4],SIGRTMIN+F2_ARRIVAL);
			send_signal(pid_list[OPE_PANE5],SIGRTMIN+F2_ARRIVAL);
		break;
		case F3_UP:
			printf("	Dang chuyen hang o tang 3.\n");
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+FINISHED);
			send_signal(pid_list[OPE_PANE3],SIGRTMIN+USING);
			send_signal(pid_list[OPE_PANE2],SIGRTMIN+F3_ARRIVAL);
			send_signal(pid_list[OPE_PANE4],SIGRTMIN+F3_ARRIVAL);
			send_signal(pid_list[OPE_PANE5],SIGRTMIN+F3_ARRIVAL);
		break;
		case F4_UP:
			printf("	Dang chuyen hang o tang 4.\n");
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+FINISHED);
			send_signal(pid_list[OPE_PANE4],SIGRTMIN+USING);
			send_signal(pid_list[OPE_PANE2],SIGRTMIN+F4_ARRIVAL);
			send_signal(pid_list[OPE_PANE3],SIGRTMIN+F4_ARRIVAL);
			send_signal(pid_list[OPE_PANE5],SIGRTMIN+F4_ARRIVAL);
		break;
		case F5_UP:
			printf("	Dang chuyen hang o tang 5.\n");
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+FINISHED);
			send_signal(pid_list[OPE_PANE5],SIGRTMIN+USING);
			send_signal(pid_list[OPE_PANE2],SIGRTMIN+F5_ARRIVAL);
			send_signal(pid_list[OPE_PANE3],SIGRTMIN+F5_ARRIVAL);
			send_signal(pid_list[OPE_PANE4],SIGRTMIN+F5_ARRIVAL);
		break;
		case F2_CALL:					//goi tang, duoc goi bang nut o vi tri cua cac tang
			printf("	Dang chuyen hang o tang 2.\n");
			send_signal(pid_list[OPE_PANE2],SIGRTMIN+FINISHEDUSING);		//nut call do->trang, den bao
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+F2_ARRIVAL);
			send_signal(pid_list[OPE_PANE3],SIGRTMIN+F2_ARRIVAL);
			send_signal(pid_list[OPE_PANE4],SIGRTMIN+F2_ARRIVAL);
			send_signal(pid_list[OPE_PANE5],SIGRTMIN+F2_ARRIVAL);
		break;
		case F3_CALL:
			printf("	Dang chuyen hang o tang 3.\n");
			send_signal(pid_list[OPE_PANE3],SIGRTMIN+FINISHEDUSING);
			send_signal(pid_list[OPE_PANE2],SIGRTMIN+F3_ARRIVAL);
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+F3_ARRIVAL);
			send_signal(pid_list[OPE_PANE4],SIGRTMIN+F3_ARRIVAL);
			send_signal(pid_list[OPE_PANE5],SIGRTMIN+F3_ARRIVAL);
		break;
		case F4_CALL:
			printf("	Dang chuyen hang o tang 4.\n");
			send_signal(pid_list[OPE_PANE4],SIGRTMIN+FINISHEDUSING);
			send_signal(pid_list[OPE_PANE2],SIGRTMIN+F4_ARRIVAL);
			send_signal(pid_list[OPE_PANE3],SIGRTMIN+F4_ARRIVAL);
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+F4_ARRIVAL);
			send_signal(pid_list[OPE_PANE5],SIGRTMIN+F4_ARRIVAL);
		break;
		case F5_CALL:
			printf("	Dang chuyen hang o tang 5.\n");
			send_signal(pid_list[OPE_PANE5],SIGRTMIN+FINISHEDUSING);
			send_signal(pid_list[OPE_PANE2],SIGRTMIN+F5_ARRIVAL);
			send_signal(pid_list[OPE_PANE3],SIGRTMIN+F5_ARRIVAL);
			send_signal(pid_list[OPE_PANE4],SIGRTMIN+F5_ARRIVAL);
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+F5_ARRIVAL);
		break;
		default:

		break;
	}
}
void lift_arrival(int sigNo){//  hàm này xử lí ngắt nhận được từ liftCtrl:
	int sigNumber;
	switch(sigNo-SIGRTMIN){
		case FINISHED:
			send_finish_notification(sigNo);
			break;
		case F1_ARRIVAL:
			//printf("cur req: %d\n",current_request );
			send_signal(pid_list[OPE_PANE1],SIGRTMIN+USING);
			send_signal(pid_list[OPE_PANE2],sigNo);
			send_signal(pid_list[OPE_PANE3],sigNo);
			send_signal(pid_list[OPE_PANE4],sigNo);
			send_signal(pid_list[OPE_PANE5],sigNo);
			if(read(fifoFd, &sigNumber, sizeof(int)) > 0){
				// printf("get queue dc: %d\n",sigNumber );
				sleep(WAIT_TIME);		//delay luc nhan hang o tang 1 neu trong fifo co request
				lift_is_moving=send_signal(pid_list[LIFT_CTR],sigNumber);
				current_request=sigNumber;
			}
			else{
				lift_is_moving=FALSE;
				printf("Lift stopped!\n");
			}
			break;
		case F2_ARRIVAL:
			// break;
		case F3_ARRIVAL:
			// break;
		case F4_ARRIVAL:
			// break;
		case F5_ARRIVAL:
			send_signal(pid_list[OPE_PANE1],sigNo);
			send_signal(pid_list[OPE_PANE2],sigNo);
			send_signal(pid_list[OPE_PANE3],sigNo);
			send_signal(pid_list[OPE_PANE4],sigNo);
			send_signal(pid_list[OPE_PANE5],sigNo);
			break;
		default:
			lift_is_moving=TRUE;
		break;
	}
}
int main(int argc, char const *argv[])
{
	signal(SIGRTMIN+F2_CALL,up_request);	signal(SIGRTMIN+F3_CALL,up_request);	signal(SIGRTMIN+F4_CALL,up_request);	signal(SIGRTMIN+F5_CALL,up_request);
	signal(SIGRTMIN+F2_UP,up_request);	signal(SIGRTMIN+F3_UP,up_request);	signal(SIGRTMIN+F4_UP,up_request);	signal(SIGRTMIN+F5_UP,up_request);
	signal(SIGRTMIN+F1_ARRIVAL,lift_arrival);signal(SIGRTMIN+F2_ARRIVAL,lift_arrival);signal(SIGRTMIN+F3_ARRIVAL,lift_arrival);signal(SIGRTMIN+F4_ARRIVAL,lift_arrival);signal(SIGRTMIN+F5_ARRIVAL,lift_arrival);
	signal(SIGRTMIN+FINISHED,lift_arrival);
	// signal(SIGCHLD, SIG_IGN);
	// signal(SIGRTMIN+USING, SIG_IGN);
	pid_list=update_pid(LIFT_MNG);
	setpgid(pid_list[LIFT_MNG],0);
	printf("mng_process_id: %d\n",getpid());

	//Make FIFO file
	remove(FIFO_FILE_PATH);
	if ( mkfifo(FIFO_FILE_PATH,0666) == -1 ){
	    perror("mkfifo");
	    exit(1);
	}
	//Open for writing only
	if ( ( fifoFd=open(FIFO_FILE_PATH,O_RDWR|O_NONBLOCK) ) == -1 ){
	    perror("fifofile open");
	    exit(1);
	}

	while(1){
		sleep(1);
	}
	close(fifoFd);
	unlink(FIFO_FILE_PATH);
	return 0;
}
