#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>

//Initialize exit variable to quit threads
int shouldExit = 0;

//Define Semaphores and Mutexes
sem_t empty;
sem_t full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;;

//Define Buffer
typedef int buffer_item;
#define BUFFER_SIZE 5
buffer_item buffer[BUFFER_SIZE];


void initBuffer(){
	printf("Setting up buffer....\n");
	sleep(0);
	//buffer = (buffer_item [5]);
	printf("Buffer Initialized\n");
}

int insert_item(buffer_item item,long id){
	int success = 0;
	//Insert item into buffer
	//Wait for something to be consumed
	printf("Producer thread %lu waiting for empty spot in buffer\n",id);
	sem_wait(&empty);
	pthread_mutex_lock(&mutex);
	printf("Producer thread %lu has lock on empty spot in buffer\n",id);
	int emptySpots = 0;
	sem_getvalue(&empty,&emptySpots);
	buffer[emptySpots] = item;
	pthread_mutex_unlock(&mutex);
	sem_post(&full);
	success = 1;
	
	if(success){return 1;}
	else{return 0;}
}

int remove_item(buffer_item *item,long id){
	int success = 0;
	//Remove Item from buffer
	//Wait for something to be produced
	printf("Consumer thread %lu waiting for production to finish\n",id);
	sem_wait(&full);
	pthread_mutex_lock(&mutex);
	printf("Consumer thread %lu has a lock on an item for consumption\n",id);
	int fullSpots = 0;
	sem_getvalue(&full,&fullSpots);
	*item = buffer[BUFFER_SIZE - fullSpots - 1];
	buffer[BUFFER_SIZE - fullSpots-1] = 0;
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);
	success = 1;
	
	if(success){return 1;}
	else{return 0;}
}	

void *initProducer(void *t){
	buffer_item item = 0;
	long id = (long)t;
	while(1){
		if(shouldExit){pthread_exit(0);}
		//Sleep random period of time
		sleep(rand()%3);
		//Genereate random number for item size
		int temp = rand();
		item = temp;
		//Insert and item
		if(insert_item(item,id)){printf("Production Successful for thread %lu\n\n\n",id);}
		else{printf("Error in production on thread %lu\n",id);}
	}
}
void *initConsumer(void *t){
	buffer_item item = 0;
	long id = (long)t;
	while(1){
		if(shouldExit){pthread_exit(0);}
		//Sleep random period of time
		sleep(rand()%3);
		//Remove an item
		if(remove_item(&item,id)){printf("Consumer consumed %d on thread %lu\n\n\n",item,id);}
		else{printf("Error in consumption on thread %lu\n",id);}
	}
}

int main(int argc,char *argv[]){
	//Get Command Line Args
	int sleepTime = atoi(argv[1]);
	int pThreads = atoi(argv[2]);
	int cThreads = atoi(argv[3]);
	//Initialize Buffer
	initBuffer();

	//Initialize Semaphores and Mutexes
	sem_init(&empty,0,5);
	sem_init(&full,0,0);
	pthread_mutex_init(&mutex,NULL);

	
	//Make Consumer and Producer Threads
	pthread_t consumerThreads[cThreads];
	pthread_t producerThreads[pThreads];
	long t;
	int ret;
	for(t = 0; t<cThreads;t++){
		ret = pthread_create(&consumerThreads[t],NULL,initConsumer,(void *)t);
	}
	for(t = 0; t<pThreads;t++){
		ret = pthread_create(&producerThreads[t],NULL,initProducer,(void *)t);
	}
	
	
	/*
	//Join Threads at the end
		for(t = 0; t<cThreads;t++){
		pthread_join(consumerThreads[t],NULL);
	}
	for(t = 0; t<pThreads;t++){
		pthread_join(producerThreads[t],NULL);
	}*/
	
	
	//Sleep before terminating
	sleep(sleepTime);
	shouldExit = 1;
	
	return 0;	
}
