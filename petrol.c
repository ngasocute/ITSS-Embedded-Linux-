#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#define total_cars 7
#define total_attendes 3
#define total_petrols 3
#define queue_len 4

pthread_t threads[100008];
pthread_t atm;
sem_t max_capacity;
int free_petrols = total_petrols;
sem_t mutex2;
int free_queue_pos=queue_len;
sem_t mutex_queue;
sem_t sem_queue;
sem_t sem_petrol;
int petrol[total_petrols];
sem_t car_ready;

sem_t finished[total_cars];
sem_t done[total_cars];
sem_t finishedcounter;
sem_t leave_petrol[total_petrols];

sem_t mutex3;
sem_t mutex4;

sem_t readywithpayment;

sem_t receipt[total_cars];

int pipe1[2],pipe2[2];

void write_pipe(int writefd,int value)
{
	if(write(writefd,&value,sizeof(int)) != sizeof(int)) {
		printf("pipe write error");
		exit(0);
	}
}


void read_pipe(int readfd,int *value,int *status)
{
	int n;
	if( (n =  read(readfd,value,sizeof(int))) == -1) {
		printf ("pipe read error");
		exit(0);
	}
	*status = (char)n;
}



void AttenderMaker();
void Attender(int number);
void serveCar(int myCar,int number,int mypetrol);
void acceptPayment();
void CarMaker();
void Car(int CarId);
void ATM();

int main() { 
	int iseed=time(NULL);
	srand(iseed);
	int i;
	sem_init(&max_capacity,0,total_cars);
	sem_init(&mutex2,0,1);
	sem_init(&mutex3,0,1);
	sem_init(&mutex4,0,1);
	sem_init(&mutex_queue,0,1);
	sem_init(&sem_queue,0,queue_len);
	sem_init(&sem_petrol,0,total_petrols);
	sem_init(&car_ready,0,0);
	sem_init(&finishedcounter,0,0);
	sem_init(&readywithpayment,0,0);

	for(i=0;i<total_cars;i++)
	{ 
		sem_init(&finished[i],0,0);
		sem_init(&receipt[i],0,0);
	}
	for(i=0;i<total_petrols;i++)
	{
		sem_init(&leave_petrol[i],0,0); petrol[i]=-1;
	}

	if (pipe(pipe1) < 0) printf("can't create pipe1\n"); 
	if (pipe(pipe2) < 0) printf("can't create pipe2\n"); 

	// pthread_create(&threads[i],NULL,(void *)&ATM,(void *)i);
	AttenderMaker();
	pthread_create(&atm,NULL,(void *)&ATM,NULL);
	CarMaker();
    sleep(3);
    printf("Finish, Time to go home\n");
}

void ATM() {
	int Car;
	int MyId = -1;
	int Mychairno = 0; /*we treat the cash counter is a special petrol */
	int status;
	while(1) {
		/* wait for a customer to arrive here with money in hand */
		sem_wait(&readywithpayment);

		/* good to see money. let's see who the guy is */
		sem_wait(&mutex4);
		read_pipe(pipe2[0],&Car,&status);
		sem_post(&mutex4);


		printf("ATM: Car %d has arrived with payment. Calling a attender\n",Car);

		sem_wait(&mutex3);

		write_pipe(pipe1[1],MyId);
		write_pipe(pipe1[1],Mychairno);
		sem_post(&mutex3);
		sem_post(&car_ready);

		printf("ATM: Waiting to hear that Car %d has paid to a attender\n",Car);
		sem_wait(&finishedcounter);
		printf("nPayment started\n");
		sleep(0.5);
		printf("ATM: Car %d has paid to a attender\n",Car);

		sem_post(&receipt[Car]);
	} 
}

void AttenderMaker()
{
	int i=1;
	while (i <= total_attendes) {
		pthread_create(&threads[i],NULL,(void *)&Attender,(void *)i);
		i++;
	}

}

void CarMaker()
{
	int i=0;
	while (i < total_cars) {
        int ran = rand()%3;
        
		sleep(ran);
		pthread_create(&threads[i+total_attendes],NULL,(void *)&Car,(void *)i);
		printf("Car %d born\n",i );
        printf("@@@@ Random coming time: %d\n", ran);
		i++;
	}

}

void Attender(int number) {
	int myCar, mypetrol, status;
	while(1) {

		printf("Attender %d: waiting for a car\n",number);
		/* we sleep till there is there a car on a petrol
		   or there is a call from the atm */
		sem_wait(&car_ready);

		/* obtain customerid and petrol_no securely */
		sem_wait(&mutex3);

		read_pipe(pipe1[0],&myCar,&status);
		read_pipe(pipe1[0],&mypetrol,&status);
		sem_post(&mutex3);

		if (myCar != -1) {
			serveCar(myCar,number,mypetrol);
		}
		else {
			acceptPayment(); 
		}
	} 
}

void acceptPayment()
{
	sem_post(&finishedcounter);
}

void serveCar(int myCar,int number,int mypetrol)
{
	printf("Attender %d fills gas in Car %d on petrol %d\n",number,myCar,mypetrol);

	printf("nFilling started\n");
	sleep(1);
	printf("filling completed\n");
	printf("##############Filling finished of Car %d by Attender %d on petrol %d\n",myCar,number,mypetrol);

	sem_post(&finished[myCar]);

	sem_wait(&leave_petrol[mypetrol]);

	printf("Attender %d has seen  Car %d leave petrol %d and go to ATM\n",number,myCar,mypetrol);
	/* now sem_post that a petrol is free */
	sem_post(&sem_petrol);
}
void Car(int CarId) {
	int i,k;

	/* enterStation begins */
	sem_wait(&max_capacity);
	printf("Car %d enters petrol\n",CarId);
	/* enterStation ends */
	/* waitInLine starts */
	sem_wait(&mutex2);
	sem_wait(&mutex_queue);
	if ((free_petrols == 0) || (free_queue_pos < queue_len)) {
		sem_post(&mutex_queue);
		sem_post(&mutex2);
		printf("Car %d deciding petrol quality :)\n",CarId);
		sem_wait(&mutex_queue);
		if (free_queue_pos <= 0) {
			printf("Car %d waiting\n",CarId);
		}
		free_queue_pos--;
		sem_post(&mutex_queue);
		sem_wait(&sem_queue); 

		printf("Car %d is in queue now\n",CarId);
		sem_wait(&sem_petrol);
		/* waitInLine ends */
		/* goTopetrol starts */
		sem_wait(&mutex_queue);
		free_queue_pos++;
		sem_post(&mutex_queue);
		sem_post(&sem_queue);
		printf("Car %d released its queue position\n",CarId);
		/* goTopetrol ends */
	}
	else {
		sem_post(&mutex_queue);sem_post(&mutex2);sem_wait(&sem_petrol);
	}
	/* We wait for petrol here */
	sem_wait(&mutex2);
	i = 0;
	while ((petrol[i] != -1) && (i < total_petrols)) i++;
	if (i == total_petrols) {
		printf("this is not possible! we are here because at least 1 petrol empty!");
		exit(0);
	}
	else {
		petrol[i] = CarId;
		free_petrols--; 
		printf("Car %d occupies petrol %d. free_petrols = %d\n",CarId,i,free_petrols);
	}
	sem_post(&mutex2);

	sem_wait(&mutex3);
	//		Filling started
	printf("Filling for %d started\n",CarId);
	write_pipe(pipe1[1],CarId);
	write_pipe(pipe1[1],i);
	sem_post(&mutex3);
	sem_post(&car_ready);

	//Filling done
	sem_wait(&finished[CarId]);
	printf("Filling for %d started",CarId);

	sem_wait(&mutex2);
	free_petrols++;
	petrol[i] = -1;
	sem_post(&leave_petrol[i]);
	sem_post(&mutex2);
	printf("Car %d left petrol %d to atm. free_petrols = %d\n",CarId,i,free_petrols); 

	/*want to pay. let's put our id on to a pipe to the ATM*/
	sem_wait(&mutex4);
	write_pipe(pipe2[1],CarId);
	sem_post(&mutex4);
	/* Pay starts */
	printf("Car %d ready to pay\n",CarId);

	sem_post(&readywithpayment);
	sem_wait(&receipt[CarId]);

	printf("Car %d paid\n\n\n",CarId);
	sem_post(&done[CarId]);
	/* Pay ends */
	sem_post(&max_capacity);
	if (CarId == (total_cars-1)) {
		for(k=0;k <= (total_cars-1); k++) sem_wait(&done[k]);
		puts("All cars served");
		exit(0);
	}
	else{
      
        pthread_exit(0);  //exit station

    } 
}
