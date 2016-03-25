#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#define MAX_RESOURCES 100

int numThreads = 1;
int available_resources = MAX_RESOURCES;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t resc_thresh;

void waitForResources(int resc){
	while(resc > available_resources){
		sleep(.5);
	}
}

//Part A of the code
int decrease_count(int count,long t){
	if(available_resources < count){
		printf("XXXXXXX Insufficient Resources for thread %lu\n",t);
		printf("XXXXXXX Tried to claim %d resources\n", count);
		printf("XXXXXXX Available: %d\n\n", available_resources);
		return -1;
	}
	else{
		available_resources -= count;
		printf("Thread %lu claimed %d resources\n",t,count);
		printf("Claimed: %d resources\n", count);
		printf("Available: %d\n", available_resources);
		
		return 0;
	}
}
int increase_count(int count,long t){
	available_resources += count;
	printf("Thread %lu added %d resources\n",t,count);
	printf("There are now %d resources available\n\n\n",available_resources);
	return 0;
}


void *getResources(void *t){
	long threadID = (long)t;
	int resc = rand()%100;
	if(resc > MAX_RESOURCES){
		printf("Thread %lu attempted to claim more than max resources.\n",threadID);
	}
	else{
		int success = decrease_count(resc,threadID);
		if(success == 0){increase_count(resc,threadID);}
	}
	pthread_exit(0);
}



//This is the code where the monitor is implemented
//Part C of the code
void decrease_countMonitor(int resc,long t){
	pthread_mutex_lock(&mutex);
	if(available_resources < resc){
		printf("Thread %lu WAITING to claim %d resources\n\n",t, resc);
		pthread_cond_wait(&resc_thresh, &mutex);
	}
	available_resources -= resc;
	printf("Thread %lu\n",t);
	printf("Claimed: %d resources\n", resc);
	printf("Available: %d\n\n\n", available_resources);
	pthread_mutex_unlock(&mutex);
}

void increase_countMonitor(int resc, long t){
	pthread_mutex_lock(&mutex);
	available_resources += resc;
	printf("Thread %lu added %d resources\n",t,resc);
	printf("There are now %d resources available\n\n\n",available_resources);
	pthread_cond_signal(&resc_thresh);
	pthread_mutex_unlock(&mutex);
}


void *getResourcesMonitor(void *t){
	long threadID = (long)t;
	int resc = rand()%10+90;
	if(resc > MAX_RESOURCES){
		printf("Thread %lu attempted to claim more than max resources.\n",threadID);
	}
	else{
		decrease_countMonitor(resc,threadID);
		increase_countMonitor(resc,threadID);
	}
	pthread_exit(0);
}


	
	
	
	
	
	
	
	
	
	
	
int main(int argc, char *argv[])
{
	numThreads = atoi(argv[1]);
	printf("Enter A to see race condition or M to see the Monitor: ");
	char condition[80];
	fgets(condition,80,stdin);
	printf("%s",condition);

	//Make all the threads
	pthread_cond_init(&resc_thresh,NULL);
	pthread_t threads[numThreads];
	int ret;
	long t;
	
	if(strcmp(condition,"A\n") == 0 || strcmp(condition,"a\n") == 0){
		printf("RACE CONDITION:\n");
		for(t = 0; t<numThreads;t++){
			ret = pthread_create(&threads[t],NULL,getResources,(void *)t);
		}

		//Make threads wait for all to terminate before ending program
		for(t = 0; t<numThreads;t++){
			pthread_join(threads[t],NULL);
		}
	}
	else{
		if(strcmp(condition,"M\n") == 0 || strcmp(condition,"m\n") == 0){
			printf("Monitor\n");
			for(t = 0; t<numThreads;t++){
				ret = pthread_create(&threads[t],NULL,getResourcesMonitor,(void *)t);
			}

			//Make threads wait for all to terminate before ending program
			for(t = 0; t<numThreads;t++){
				pthread_join(threads[t],NULL);
			}
		}
	}
	pthread_exit(NULL);
}
