#include "sigs.h"
// Như ta đã biết, hệ thống sử dụng một share memory để lưu
// trữ pid của các chương trình, khi mỗi chương trình được
// chạy thì sẽ gọi đến hàm này để cập nhật pid của nó và vị
// trí thứ i trong share memory:
pid_t* update_pid(int i){
	int shmid = shmget(SHARE_KEY,10*sizeof(pid_t),IPC_CREAT);
	pid_t *shmpointer = (pid_t*)shmat(shmid,0,0);
	if(shmpointer==(int*)-1){perror("shmat");exit(0);}
	shmpointer[i]=getpid();
	return shmpointer;
}
void release_shm(){
	int shmid = shmget(SHARE_KEY,10*sizeof(pid_t),IPC_CREAT);
	shmctl(shmid,IPC_RMID,NULL);
}
int send_signal(pid_t pid,int sigNo){
	if (pid>0) return (kill(pid,sigNo)==0);
	else return 0;
}