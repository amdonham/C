#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#define MAX_RESOURCES 6

int numThreads = 10;
int resc = 6;
int available_resources = MAX_RESOURCES;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int decrease_count(int count,long t){
	if(available_resources < count){printf("Insufficient Resources for thread %lu\n",t);return -1;}
	else{available_resources -= count; printf("Thread %lu claimed some resources\n",t); return 0;}
}
int increase_count(int count,long t){
	available_resources += count;
	return 0;
}

int decrease_countFixed(int count,long t){
	if(available_resources < count){printf("Insufficient Resources for thread %lu\n",t);return -1;}
	else{available_resources -= count; printf("Thread %lu claimed some resources\n",t); return 0;}
}
int increase_countFixed(int count,long t){
	available_resources += count;
	return 0;
}


void *getResources(void *t){
	long threadID = (long)t;
	if(resc > MAX_RESOURCES){printf("Thread %lu attempted to claim more than max resources.\n",threadID);}
	else{
		decrease_count(resc,threadID);
		increase_count(resc,threadID);
	}
	pthread_exit(0);
}

void *getResourcesFixed(void *t){
	long threadID = (long)t;
	pthread_mutex_lock(&mutex);
	if(resc > MAX_RESOURCES){printf("Thread %lu attempted to claim more than max resources.\n",threadID);}
	else{
		decrease_countFixed(resc,threadID);
		increase_countFixed(resc,threadID);
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(0);
}

int main()
{
	//Make all the threads
	pthread_t threads[numThreads];
	int ret;
	long t;
	printf("RACE CONDITION:\n");
	for(t = 0; t<numThreads;t++){
		ret = pthread_create(&threads[t],NULL,getResources,(void *)t);
	}
	
	//Make threads wait for all to terminate before ending program
	for(t = 0; t<numThreads;t++){
		pthread_join(threads[t],NULL);
	}
	
	printf("FIXED RACE CONDITION\n");
	for(t = 0; t<numThreads;t++){
		ret = pthread_create(&threads[t],NULL,getResourcesFixed,(void *)t);
	}
	
	//Make threads wait for all to terminate before ending program
	for(t = 0; t<numThreads;t++){
		pthread_join(threads[t],NULL);
	}
		
	pthread_exit(NULL);
}
