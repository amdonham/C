#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

long numAdds = 0; 
int *page;
int *offset;
long *pageTable;
long* physical;


void getPhysical(long *logicals){
	physical = malloc(sizeof(long)*1000);
	int i;
	long frame = 0;
	for(i = 0;i<numAdds;i++){
		int off = offset[i];
		if(pageTable[page[i]] == 1){
			pageTable[page[i]] = frame; 
			frame += 256;
		}
		int frameNum = pageTable[page[i]];
		physical[i] = frameNum + off;
	}
}

void initPageTable(){
	pageTable = malloc(256*sizeof(long));
	int i;
	pageTable[0] = 0;
	for(i = 1; i < 256;i++){
		pageTable[i] = 1;
	}
}

void toDec(int *bin,int index){
	int dec = 0;
	int i;
	int pow = 1;
	for(i = 15;i >= 0;i--){
		if(i >7){
			if(bin[i] == 1){
				dec += pow;
			}
			pow = pow*2;
		}
		if(i == 7){pow = 1; offset[index] = dec; dec = 0;}
		if(i <= 7){
			if(bin[i] == 1){
				dec += pow;
			}
			pow = pow*2;
		}
	}	
	page[index] = dec;
}

void toBinary(long *adds){
	int i;
	page = malloc(sizeof(long)*numAdds);
	offset = malloc(sizeof(long)*numAdds);

	for( i = 0; i < numAdds; i++){
		long num = adds[i];
		int index = 15;
		int b[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		while(num != 0){
			b[index] = num%2;
			num = num/2;
			index -= 1;
		}
		toDec(b,i);

	}
}

long * openFile(char* filename){
	//Open file
	FILE *fp;
	fp = fopen(filename,"r");
	long *addresses = malloc(sizeof(long)*1000);
	if(fp==NULL){printf("File not found.\n");}
	else{

		int i = 0;
		char ch;
		char l[5];
		int j = 0;
		while((ch = fgetc(fp))!= EOF){
			if(ch == '\n'){
				j = 0;
				addresses[i] = atoll(l);
				numAdds++;
				i++;
				int k;
				for(k = 0;k<5;k++){
					l[k] = 0;
				}
			}
			else{l[j] = ch;j++;}
		}
	}
	return addresses;
}

int main(int argc,char *argv[]){
	//Get Command Line Args
	char* addresses = argv[1];
	//Open file and read addresses
	long *logicals = openFile(addresses);
	toBinary(logicals);
	initPageTable();
	getPhysical(logicals);
	//Print out logical addresses
	int i;
	for(i = 0; i < numAdds;i++){
		printf("%lu : ",logicals[i]);
		printf("%d ",page[i]);

		printf("%lu",physical[i]);
		printf("\n");
		
	}
	return 0;
}
