#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

long numAdds = 0; 
int *page;
int *offset;
long *pageTable;
long* physical;
int pageFault = 1;
int tlbHits = 0;
long **tlb;

long checkTlb(int index){
	int i;
	for(i = 0;i < 16; i++){
		if(tlb[i][0] == page[index]){;
			return tlb[i][1];
		}
	}
	
	return (long)2;
}

void addTlb(int index){
	int i;
	for(i = 1;i < 16; i++){
		tlb[i-1][0] = tlb[i][0];
		tlb[i-1][1] = tlb[i][1];
	}
	tlb[15][0] = page[index];
	tlb[15][1] = pageTable[page[index]];
}

void getPhysical(long *logicals){
	physical = malloc(sizeof(long)*1000);
	int i;
	long frame = 0;
	for(i = 0;i<numAdds;i++){
		int off = offset[i];
		long isTlb = checkTlb(i);
		if(isTlb == 2){	
			if(pageTable[page[i]] == 1){
				pageTable[page[i]] = frame; 
				frame += 256;
				pageFault++;
			}
			physical[i] = pageTable[page[i]] + off;
			addTlb(i);
		}
		else{physical[i] = isTlb + off;tlbHits++;}
		
	}
}

int isFreeFrame(){
	int i;
	for(i = 0;i < 128;i++){
		if(pageTable[i] == 1){return i;}
	}
	return 0;
}

int findVictim(){
	int i;
	for(i = 0; i < 128; i++){
		int intlb = 0;
		int j;
		for(j = 0; j < 16; j++){
			if(tlb[j][0] == i){intlb = 1;}
		}
		if(!intlb){return i;}
	}
}
void getPhysicalMod(long *logicals){
	physical = malloc(sizeof(long)*1000);
	int i;
	long frame = 0;
	for(i = 0;i<numAdds;i++){
		int off = offset[i];
		long isTlb = checkTlb(i);
		if(isTlb == 2){	
			if(pageTable[page[i]] == 1){
				pageTable[page[i]] = frame; 
				frame += 128;
				pageFault++;
			}
			else{
				
				int freeFrame = isFreeFrame();
				//Use a free frame if available
				if(freeFrame>0){
					pageTable[freeFrame] = frame;
					frame += 128;
					pageFault++;
				}
				//If no free frame find first frame no in use by tlb and make it victim
				else{
					int victim = findVictim();	
					pageTable[victim] = frame;
					frame += 128;
					pageFault++;
				}
			}
			physical[i] = pageTable[page[i]] + off;
			addTlb(i);
		}
		
		else{physical[i] = isTlb + off;tlbHits++;}
		
	}
}

void initPageTable(){
	pageTable = malloc(256*sizeof(long));
	int i;
	pageTable[0] = 0;
	for(i = 1; i < 256;i++){
		pageTable[i] = 1;
	}
	tlb = malloc(sizeof(long)*16);
	for(i = 0;i < 16;i++){
		tlb[i] = malloc(2*sizeof(long));
		tlb[i][0] = 256;
		tlb[i][1] = 0;
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
	//Prompt for Mod
	printf("Input '1' for Modifications and '0' for No Modifications: ");
	char input[80];
	fgets(input,80,stdin);
	int mod = atoi(input);
	if(mod == 1){mod = 1;}
	//Open file and read addresses
	long *logicals = openFile(addresses);
	toBinary(logicals);
	initPageTable();
	if(mod){getPhysicalMod(logicals);}
	else{getPhysical(logicals);}
	//Print out logical addresses
	int i;
	for(i = 0; i < numAdds;i++){
		printf("Virtual address: %lu ",logicals[i]);
		printf("Physical address: %lu ",physical[i]);
		printf("Value: %d",*((signed char *)&physical[i]));
		printf("\n");
		
	}
	printf("Number of Translated Addresses = %lu\n",numAdds);
	printf("Page Faults = %d\n",pageFault);
	printf("Page Fault Rate = %g\n", (double)pageFault/(double)numAdds);
	printf("TLB Hits = %d\n",tlbHits);
	printf("TLB Hit Rate = %g\n", (double)tlbHits/(double)numAdds);
	return 0;
}
