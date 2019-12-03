/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
pid_t Fg_pid;
void* jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE]; 
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	   

	
	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */
	struct sigaction sa_int;
	sa_int.sa_handler = &handler_int;
	sigfillset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	
	struct sigaction sa_stp;
	sa_stp.sa_handler = &handler_stp;
	sigfillset(&sa_stp.sa_mask);
	sa_stp.sa_flags = 0;
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */
	if (!sigaction(SIGINT, &sa_int, NULL)) quit(-1);
	if (!sigaction(SIGSTP, &sa_stp, NULL)) quit(-1);
	/************************************/

	/************************************/
	// Init globals 


	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1)); //free?
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
	Fg_pid = 0;
	
	jobs = (void*)&Vjobs;
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
		strcpy(L_Fg_Cmd, cmdString);
		pid_t fork_res;
		fork_res = fork();
		switch(Fg_pid){
			case 0:{ // Child process
					// perform a complicated Command
				if(!ExeComp(lineSize)) kill(getpid(), SIGKILL); 
					// background command	
				if(!BgCmd(lineSize, cmdString)) kill(getpid(), SIGKILL); 
					// built in commands
				ExeCmd(jobs, lineSize, cmdString);
				kill(getpid(), SIGKILL);
			}
			
			case -1:{ // Failed
				break;
			}
			
			default:{
				Fg_pid = fork_res;
				waitpid(fork_res, NULL, 0);
			}
		/* initialize for next line read*/
		if(L_Fg_Cmd[0] == '\0'){
			Job *addedJob = new Job(Fg_pid, string(cmdString));
			Vjobs.pushback(*addedJob);
		}
		Fg_pid = 0;
		L_Fg_Cmg[0]='\0';
		lineSize[0]='\0';
		cmdString[0]='\0';
		
	}
    return 0;
}

