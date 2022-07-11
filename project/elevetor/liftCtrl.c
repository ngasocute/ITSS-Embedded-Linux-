#include "sigs.h"

pid_t *pid_list,sensor_process_id,body_process_id;
int des=1,current_floor=1;
// Hàm xử lí ngắt khi nhận lệnh yêu cầu chuyển hàng từ liftMng:
void up_request(int sigNo){	
	//printf("up_request_get %d \n",sigNo );
	switch(sigNo-SIGRTMIN){
		case F1_UP:
		case F1_CALL: des=1;
		printf("Back to floor %d\n",des);
		send_signal(body_process_id,SIGRTMIN+LIFT_DOWN);
		
		break;
		case F2_UP:
		case F2_CALL: des=2;
		printf("Bat dau di chuyen len tang %d\n",des);
		send_signal(body_process_id,SIGRTMIN+LIFT_UP);
		
		break;
		case F3_UP:
		case F3_CALL: des=3;
		printf("Bat dau di chuyen len tang %d\n",des);
		send_signal(body_process_id,SIGRTMIN+LIFT_UP);
		
		break;
		case F4_UP:
		case F4_CALL: des=4;
		printf("Bat dau di chuyen len tang %d\n",des);
		send_signal(body_process_id,SIGRTMIN+LIFT_UP);
		
		break;
		case F5_UP:
		case F5_CALL: des=5;
		printf("Bat dau di chuyen len tang %d\n",des);
		send_signal(body_process_id,SIGRTMIN+LIFT_UP);
		
		break;
		default: break;		
	}
	
}
// Hàm xử lí ngắt khi nhận được tín hiệu(từ liftSensor) báo thang máy đến một tầng nào đó:
void sensor_change(int sigNo){	
	// printf("sensor_change_get %d \n",sigNo );
	int i;			
	switch(sigNo-SIGRTMIN){
		case F1_ARRIVAL:
		send_signal(pid_list[LIFT_MNG],sigNo);		
		if(des==1){
			send_signal(body_process_id,SIGRTMIN+LIFT_STOP);
			//send_signal(pid_list[LIFT_MNG],SIGRTMIN+FINISHED);
			//printf("Ctr send Mng: %d\n",SIGRTMIN+FINISHED);									
			puts("Hoan thanh nhiem vu.");			
		}
		else{
			
		}		
		break;
		case F2_ARRIVAL:		
		case F3_ARRIVAL:		
		case F4_ARRIVAL:		
		case F5_ARRIVAL:
			i=sigNo-SIGRTMIN-F1_ARRIVAL+1;			
			if(des==i){
				send_signal(body_process_id,SIGRTMIN+LIFT_STOP);				
				send_signal(pid_list[LIFT_MNG],SIGRTMIN+FINISHED);
				sleep(WAIT_TIME);				
				des=1;
				send_signal(body_process_id,SIGRTMIN+LIFT_DOWN);							
			}
			else{
				send_signal(pid_list[LIFT_MNG],sigNo);
			}			
			break;		
		default:			
		break;
	}	
}
//--------------------------------------------------------------
/* Tiến trình liftSensor thực hiện công việc:
	Liên tục check vị trí thang máy( cái mà dc lưu trong bộ nhớ
	chia sẻ). nếu thấy vị trí thang máy ứng với giá trị tưong
	ứng của từng tầng thì gửi tín hiệu về liftCtrl báo rằng
	thang máy vừa đi qua tầng ấy:
*/
void sensor_process_run(){	
	int control_process_id=getppid();
	//printf("Sensor: %d of  Ctrl: %d \n",getpid(),control_process_id);
	int previous_position=15;	
	
	while(1){
		usleep(CLOCK);
		
		if(previous_position ^ pid_list[LIFT_POSITION]){// nếu vị trí của thang máy bị thay đổi.
			switch(pid_list[LIFT_POSITION]){			
				case 15:			 
				//gửi tín hiệu (báo thang máy đang ở tầng 1) tới tiến trình liftCtrl: 
				send_signal(control_process_id,SIGRTMIN+F1_ARRIVAL);break;
				case 45:
				//gửi tín hiệu (báo thang máy đang ở tầng 2) tới tiến trình liftCtrl:
				send_signal(control_process_id,SIGRTMIN+F2_ARRIVAL);break;
				case 75: 
				
				send_signal(control_process_id,SIGRTMIN+F3_ARRIVAL);break;
				case 105: 
				
				send_signal(control_process_id,SIGRTMIN+F4_ARRIVAL);break;
				case 135: 
				
				send_signal(control_process_id,SIGRTMIN+F5_ARRIVAL);break;			
				default:break;
			}
			previous_position=pid_list[LIFT_POSITION];// lưu lại vị trí cũ của thang máy.
		}					
	}
}// end of sensor_process

//------------------------------------------------
/* Tiến trình mà body của thang máy sẽ thực hiện:
	CHỉ nhận lệnh của tiến trình Ctrl (UP,DOWN hoặc STAND),
	và tăng giảm độ cao của thang máy. Độ cao của thang máy
	chỉ nằm trong khoảng 15(tầng 1) đến 135(tầng 5) và được lưu
	ở pid_list[LIFT_POSITION]
*/
#define VANTOC 5// vận tốc: 0.5m/s
int action=STAND;// hành động mà body thang máy sẽ thực hiện, lên , xuống hợac đứng yên.
void body(int sigNo){// đây là hàm xử lí của liftBody khi nhận lệnh từ liftCtrl	.
	switch(sigNo-SIGRTMIN){
		case LIFT_UP:	action=UP;		break;
		case LIFT_DOWN: action=DOWN;	break;
		case LIFT_STOP: action=STAND;	break;
		default:break;
	}
}
void body_process_run(){// đây là hàm thực hiện công việc chính của liftBody.
	int control_process_id=getppid();
	signal(SIGRTMIN+LIFT_UP,body);signal(SIGRTMIN+LIFT_DOWN,body);signal(SIGRTMIN+LIFT_STOP,body);	
	
	pid_list[LIFT_POSITION]=15;// khởi tạo vị trí ban đầu cho body thang máy
	//printf("Body: %d of  Ctrl: %d \n",getpid(),control_process_id );
	// pid_t last_value=15;// dont care, đưa thêm vào để debug trong khi làm.
	while(1){		
		usleep(CLOCK);
		switch(action){
			case DOWN:
				if(pid_list[LIFT_POSITION]>15) {
					pid_list[LIFT_POSITION]-= VANTOC;					
				}
				else action=STAND;
				break;			
			case UP:
				if(pid_list[LIFT_POSITION]<135) {
					pid_list[LIFT_POSITION]+= VANTOC;					
				}
				else action=STAND;
				break;
			default:
				break;
		}
		// dont care, đoạn này để view độ cao của body thang máy thay đổi thế nào thôi:
		// if(last_value!=pid_list[LIFT_POSITION]) {
		// 	printf("Height: %.1f metter\n",pid_list[LIFT_POSITION]/10.0);
		// 	last_value=pid_list[LIFT_POSITION];
		// }				
	}
}// end of body_process_run

//--------------------------------------------------------
/* Main của tiến trình liftCtrl thực hiện việc sau:
	Chờ nhận lệnh của tiến trình liftMng (yêu cầu thang máy 
	chuyển hàng đến tầng nào đó), rồi ra lệnh cho liftBody
	lên tầng đó xuất hàng ra, sau đó lệnh cho liftBody về tầng
	1, cuối cùng báo với liftMng là đã xong nhiệm vụ.
	Để làm dc nhiệm vụ, nó có 2 tiến trình con là liftBody và
	liftSensor( sensor ko nhận lệnh từ ai cả mà cứ liên tục 
	check pid_list[LIFT_POSTION] để xem vị trí của thang máy đến
	đâu rồi, nếu thấy đi qua tầng nào thì báo về liftCtr)
	
*/
void quit(){
	send_signal(body_process_id,SIGKILL);
	send_signal(sensor_process_id,SIGKILL);
	exit(1);
}
int main(int argc, char const *argv[]){
	
	signal(SIGRTMIN+F2_CALL,up_request);signal(SIGRTMIN+F3_CALL,up_request);signal(SIGRTMIN+F4_CALL,up_request);signal(SIGRTMIN+F5_CALL,up_request);
	signal(SIGRTMIN+F2_UP,up_request);	signal(SIGRTMIN+F3_UP,up_request);	signal(SIGRTMIN+F4_UP,up_request);	signal(SIGRTMIN+F5_UP,up_request);
	signal(SIGRTMIN+F1_ARRIVAL,sensor_change);signal(SIGRTMIN+F2_ARRIVAL,sensor_change);signal(SIGRTMIN+F3_ARRIVAL,sensor_change);signal(SIGRTMIN+F4_ARRIVAL,sensor_change);signal(SIGRTMIN+F5_ARRIVAL,sensor_change);
	
	pid_list=update_pid(LIFT_CTR);// lưu pid của tiến trình liftCtrl vào share memory.
	printf("control_process_id: %d\n",getpid());	
	if((sensor_process_id=fork())==0){
		//sinh tiến trình con liftSensor:
		sensor_process_run();
	}
	else{
		printf("sensor_process_id: %d\n",sensor_process_id );
		if((body_process_id=fork())==0){
			//sinh tiến trình con liftBody
			body_process_run();
		}
		else{
			signal(SIGINT,quit);
			printf("body_process_id: %d\n",body_process_id );
			while(1){
				sleep(5);	
			}
		}			
	}		
	return 0;
}