#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int getInput(){
	printf("Enter a Positive Number: ");
	int num;
	scanf("%d", &num);
	if(num < 0){getInput();}
	else{return num;}
}
void collatz(int num){
	printf("%d", num);
	printf(" ");
	if(num > 1){	
		if(num%2 == 0){ num = num/2;}
		else{num = (3*num)+1;}
		collatz(num);
	}
	if(num == 1){printf("\n");}//Print a new line to make answer more readable
}

int main()
{
    int pid;
    pid=fork();
    int number;
    if(pid<0)
    {
        printf("\n Error ");
        exit(1);
    }
    else if(pid==0)
    {
	//This is the child process so make it do all the work
	number = getInput();
	collatz(number);
        exit(0);
    }
    else
    {
	//This is the parent process so make it wait for child to finish
	wait();        
        exit(1);
    }

}
