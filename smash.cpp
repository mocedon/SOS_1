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
#include <vector>
#define MAX_LINE_SIZE 80
#define MAXARGS 20


int fgPid=0; // global int which its default value of zero implies that no process is running in the foreground.
string fgCmd; // this represents the command of the foreground.
vector<Job> Vjobs; //a vector of Job class.
vector<string> hist;
//a vector that holds the history namely the past commands
// up to the current point.
char prvpwd[MAX_LINE_SIZE]; //this is a char[] which represents the previous command
// given by the user.
char* L_Fg_Cmd;
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
	if (sigaction(SIGINT, &sa_int, NULL)==-1)
	{exit(-1);
	}
	if (sigaction(SIGTSTP, &sa_stp, NULL)==-1)
	{exit(-1);
	}
	/************************************/

	/************************************/
	// Init globals 


	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1)); //free?
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
	fgPid=0;
	
	//jobs = (void*)&Vjobs;
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
							  
				 
					
				 
							
					// perform a complicated Command
		if(!ExeComp(lineSize)) continue; 
					// background command	
	 	if(!BgCmd(lineSize, cmdString)) continue; //changed jobs arg to cmdString
					// built in commands
		ExeCmd(jobs, lineSize, cmdString); //jobs is instead passed by global vector Vjobs.
		
	
   
					  
		  
	
   
			
					  
							   
	
		/* initialize for next line read*/
						  
													  
							 
   
			 
				   
		lineSize[0]='\0';
		cmdString[0]='\0';
  
	}
    return 0;
}
