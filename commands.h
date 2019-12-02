#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <vector>
#include <errno.h>
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <math.h>
#include <limits.h>
#include <string.h>

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define	MAX_HISTORY_SIZE 50
using namespace std;
typedef enum { FALSE, TRUE } Bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, char* cmdString);
int ExeCmd(void* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);

class Job {
protected: // allows further implementation of inherited classes if they will be needed.
	string CMD;
	int PID;
	long int BEGINNINGTIME;
	bool STOPPED;
	
public:
	//first constructor
	Job(int pid, string cmd)
	{
		PID=pid;
		CMD=cmd;
		SetBeginningTimeToNow();
		//granting stopped a default boolean value of 'false':
		STOPPED=false;
	};
	//overload of the class constructor:
	Job(int pid, string cmd, bool stopped)
	{
		PID=pid;
		CMD=cmd;
		SetBeginningTimeToNow();
		//granting stopped a boolean value of 'stopped':
		STOPPED=stopped;
	};
	bool isStopped() { return STOPPED; } // checks whether the job is stopped or not
	long int getBeginningTime() { return BEGINNINGTIME;  }; //gets the time in which the job began
	string getcmd() { return CMD; } // gets the Cmd
	int getpid() { return PID; } //gets the pid
	void SetBeginningTimeToNow() // sets the beginning time to now
	{
		BEGINNINGTIME=time(NULL);
	}
	void setStopped(bool stopped) { STOPPED = stopped; } //changes a job's stopped status to true or false, depending on the input.
	
};

#endif
