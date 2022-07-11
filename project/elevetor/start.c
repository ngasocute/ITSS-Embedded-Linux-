#include "sigs.h"
int main(int argc, char const *argv[])
{	
	pid_t pid;	
	if((pid=fork())==0)
		execl("./mng","./mng",NULL);
	else if((pid=fork())==0)
		execl("./ctr","./ctr",NULL);
	else if((pid=fork())==0)
		execl("./opx","./opx","5",NULL);		
	else if((pid=fork())==0)
		execl("./opx","./opx","4",NULL);
	else if((pid=fork())==0)
		execl("./opx","./opx","3",NULL);
	else if((pid=fork())==0)
		execl("./opx","./opx","2",NULL);
	else	
		execl("./op1","./op1",NULL);
	return 0;
}