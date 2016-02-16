#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#define MAX_LINE 80



int main(void){
	int shouldWait = 0;
	int should_run = 1;
	char **hist = malloc(10*sizeof(char*));
	int commandcount = 0;
	//allocate the history array
	int k = 0;
	while(k < 10){
		hist[k] = malloc(MAX_LINE);
		strcpy(hist[k],"0");
		k++;
	}
	
	void addHistory(char* arg){
		int i = 0;
		while(i < 9){
			if(strcmp(hist[i+1],"0")!= 0){strcpy(hist[i],hist[i+1]);}
			i++;
		}	
		strcpy(hist[9],arg);
		commandcount++;
	}
	
	void history(){
		int i = 0;
		int count = 0;
		while(i < 10){
			if(strcmp(hist[i],"0")!= 0){
				printf("%d ", commandcount-count);
				printf("%s\n", hist[i]);
				count++;
			}
			i++;
		}
	}
	
	int runShell(){
		
		char *saveprt;
		char arg[MAX_LINE];
		char *temp;
		int argnum = 0;
		int i;
		char *tempargs[MAX_LINE/2 +1];
		char **args;
		pid_t pid;
		shouldWait = 0;
		
		printf("osh>");
		fflush(stdout);
		fflush(stdin);
		fgets(arg,MAX_LINE,stdin);
		strtok_r(arg,"\n",&saveprt);
		if(strcmp(arg,"history")!= 0 && strcmp(arg,"\n")!= 0 && arg[0]!= '!'){addHistory(arg);}
		if(arg[0] == '!'){	if(arg[1] == '!'){
								if(strcmp(hist[9],"0")!= 0){strcpy(arg,hist[9]);}
								else{printf("No commands in history\n");}}
						  	else{
								char *end;
								int n = strtol(&arg[1],&end,10);
								if(0 < n && n <= 10){
								if(strcmp(hist[10-n],"0")!= 0){strcpy(arg,hist[10-n]);}
								else{printf("No such command in history\n");}}
								else{printf("No such command in history\n");}
							}
							
		}
		temp = strtok_r(arg," ",&saveprt);
		i = 0;
		while(temp != NULL){
			tempargs[i] = temp;
			i++;
			argnum++;
			temp = strtok_r(NULL," ",&saveprt);
		}

		i = 0;
		if(strcmp(tempargs[argnum-1],"&") == 0){		
			shouldWait = 1;
			argnum --;
			args = malloc(argnum*sizeof(char*));
			while(i < argnum){	
				args[i] = malloc(MAX_LINE);
				strcpy(args[i],tempargs[i]);	
				i++;
			}
		}
		else{
			args = malloc(argnum*sizeof(char*));
			while(i < argnum){	
				args[i] = malloc(MAX_LINE);
				strcpy(args[i],tempargs[i]);	
				i++;
			}
		}	

		if(strcmp(args[0],"exit")==0){return 0;}
		else if(strcmp(args[0],"history")==0){history();free(args);runShell();}
		else{
			fflush(stdout);
			int status;
			pid = fork();
			if(pid<0){printf("Error in fork"); return 1;}
			else if(pid == 0){execvp(args[0],args);return 0;}
			else 	if(shouldWait){wait(&status);free(args);runShell();}
					else{free(args);runShell();}
		}
		

	}
	int run = runShell();
	free(hist);
	return run;
}

