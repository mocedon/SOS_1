#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <errno.h>

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
private:
	int _pid;
	string _Cmd;
	long int _beginningTime;
	bool _stopped; 

public:
	Job(int pid, string cmd, bool stopped, long int beginningTime) :
		_pid(pid), _Cmd(cmd), _stopped(stopped), _beginningTime(beginningTime) {}; //constructor
	
	int getPid() { return _pid; }
	string getCmd() { return _Cmd; }
	bool isStopped() { return _stopped; }
	long int getBeginningTime() { return _beginningTime;  };

	void setStopped(bool stopped) { _stopped = stopped; }
};

#endif
