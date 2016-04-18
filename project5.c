#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

long numAdds = 0; 
int *page;
int *offset;

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
	page = malloc(65536*numAdds);
	offset = malloc(65536*numAdds);

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
	long *addresses = malloc(65536*1000);
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
	
	
	//Print out logical addresses
	int i;
	for(i = 0; i < numAdds;i++){
		printf("%lu : ",logicals[i]);
		printf("%d ",page[i]);

		printf("%d",offset[i]);
		printf("\n");
		
	}
	return 0;
}
