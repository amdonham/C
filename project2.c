#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int *unsorted;
int *sorted;
int lines;
int half;
int odd;

int compare( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

void *sortFirstHalf(){
	int i;
	int temp1[half];
	for(i = 0;i < half;i++){
		memcpy(&temp1[i],&unsorted[i],sizeof(unsorted[i]));
	}
	qsort(temp1,half,sizeof(int),compare);
	for(i = 0;i < half;i++){
		memcpy(&unsorted[i],&temp1[i],sizeof(unsorted[i]));
	}												
	pthread_exit(0);
					}
void *sortSecondHalf(){
	int i;
	int temp2[half];
	for(i = 0;i < half-odd;i++){
		memcpy(&temp2[i],&unsorted[i+half],sizeof(unsorted[i+half]));
		
	}
	qsort(temp2,half,sizeof(int),compare);
	for(i = odd;i < half;i++){
		memcpy(&unsorted[i+half-odd],&temp2[i],sizeof(unsorted[i+half]));
	}
	pthread_exit(0);
					}
void *merge(){
	int i = 0;
	int j = 0;
	int k = half;
	while(i < lines){
		int a = 99999999;//Init to infinity as our "null" value
		int b = 99999999;
		if(j < half){a = unsorted[j];}
		if(k < lines){b = unsorted[k];}
		if(a <= b){j++; memcpy(&sorted[i],&a,sizeof(a));}
		else{k++; memcpy(&sorted[i],&b,sizeof(b));}
		i++;
	}
	pthread_exit(0);	
}


int main(int argc,char **argv){
	

	char buff[255];
	
	//read in file as a list
	//count lines
	FILE *fp;
	fp = fopen(argv[1],"r");
	fscanf(fp,"%s",buff);
	lines = 0;
	while(fgets(buff,255,(FILE*)fp)){
		lines++;
	}
	fclose(fp);
	
	//Allocate array and read in ints
	unsorted = (int*)calloc(lines, sizeof(int));
	sorted = (int*)calloc(lines,sizeof(int));
	fp = fopen(argv[1],"r");
	int i;
	for(i = 0; i < lines; i++){
		fscanf(fp,"%d",&unsorted[i]);
	}
	
	//split array into 2 arrays	
	if(lines%2 == 0){half = lines/2; odd = 0;}
	else{half = (lines+1)/2; odd = 1;}

	
	//Make 2 thread and assign a half on unsorted to be sorted in each thread
	//sort each list in its own sorting thread
	pthread_t tid1; //identifier
	pthread_attr_t attr1; //attributes
	
	pthread_attr_init(&attr1); //get default attributes
	pthread_create(&tid1,&attr1,sortFirstHalf,NULL);
	
	pthread_t tid2; //identifier 2
	pthread_attr_t attr2; //attributes 2
	
	pthread_attr_init(&attr2); //get default attributes
	pthread_create(&tid2,&attr2,sortSecondHalf,NULL);
	
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	
	
	//merge lists in a merge thread
	for(i = 0; i < lines; i++){
		//printf("%d\n",unsorted[i]);
	}
	pthread_t tid3; //identifier 2
	pthread_attr_t attr3; //attributes 2
	
	pthread_attr_init(&attr3); //get default attributes
	pthread_create(&tid3,&attr3,merge,NULL);

	pthread_join(tid3,NULL);
	
	//merge lists in a merge thread
	printf("Sorted Array: [");
	for(i = 0; i < lines; i++){
		if(i < lines-1){printf("%d,",sorted[i]);}
		else{printf("%d]\n",sorted[i]);}
	}
	free(sorted);
	free(unsorted);
	return 0;
}	

