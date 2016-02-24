#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

int TA_Sleeping;

sem_t semTA;
sem_t chairs;
pthread_mutex_t Helpmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t TAcreated = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t helping = PTHREAD_MUTEX_INITIALIZER;


void helpStudent(){
	while(1){
		int availableChairs;
		sem_getvalue(&chairs,&availableChairs);
		if(availableChairs == 3){TA_Sleeping = 1;printf("The TA is sleeping\n");}
		sem_wait(&semTA);
		sleep(2);
		pthread_mutex_unlock(&helping);
	}
}

void getHelp(long studentID){
	if(TA_Sleeping){
		TA_Sleeping = 0;
		printf("Student %lu woke up the TA\n",studentID);
		sem_post(&semTA);
		pthread_mutex_unlock(&Helpmutex);
		pthread_mutex_lock(&helping);
	}
	else{
		int availableChairs;
		sem_getvalue(&chairs,&availableChairs);
		if(availableChairs == 3){
			sem_wait(&chairs);
			printf("Student %lu took a seat in chair 1\n",studentID);
			pthread_mutex_unlock(&Helpmutex);
			sem_post(&semTA);
			pthread_mutex_lock(&helping);
			printf("TA is Helping a student %lu\n", studentID);
			sem_post(&chairs);		
			printf("Chair 1 is now open\n");
		}
		if(availableChairs == 2){
			sem_wait(&chairs);
			printf("Student %lu took a seat in chair 2\n",studentID);
			pthread_mutex_unlock(&Helpmutex);
			sem_post(&semTA);
			pthread_mutex_lock(&helping);
			printf("TA is Helping a student %lu\n", studentID);
			sem_post(&chairs);	
			printf("Chair 2 is now open\n");
		
		}
		if(availableChairs == 1){
			sem_wait(&chairs);
			printf("Student %lu took a seat in chair 3\n",studentID);
			pthread_mutex_unlock(&Helpmutex);
			sem_post(&semTA);
			pthread_mutex_lock(&helping);
			printf("TA is Helping a student %lu\n", studentID);
			sem_post(&chairs);	
			printf("Chair 3 is now open\n");		
		}
		if(availableChairs == 0){	printf("Student %lu went back to programming\n",studentID);
				pthread_mutex_unlock(&Helpmutex);}
	}
}



void *initTA(){
	sem_init(&semTA,0,0);
	sem_init(&chairs,0,3);
	TA_Sleeping = 1;
	pthread_mutex_unlock(&TAcreated);
	helpStudent();	
}
void *initStudent(void *t){
	pthread_mutex_lock(&Helpmutex);
	long id = (long)t;
	getHelp(id);
}

int main(int argc,char **argv){

	//Make n student threads
		int n = 6;
		int ret;
		//Make TA Thread
		
		
		pthread_mutex_lock(&TAcreated);
		pthread_t TA;
		ret = pthread_create(&TA,NULL,initTA,NULL);
		
		pthread_mutex_lock(&TAcreated);
		long t;
		pthread_t threads[n];
		for(t = 0; t<n;t++){
			ret = pthread_create(&threads[t],NULL,initStudent,(void *)t);
		}
		


	//Make threads wait for all to terminate before ending program
		for(t = 0; t<n;t++){
			pthread_join(threads[t],NULL);
		}
		pthread_join(TA,NULL);
	return 0;	
}
