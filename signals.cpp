// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

extern int fgPid;
extern char* L_Fg_Cmd;
void handler_int(int sig_num){
	printf("smash > signal SIGINT was sent to pid %d\n",getpid());
	if (!L_Fg_Cmd[0]) return;
	fflush(stdout);
	kill(fgPid, SIGKILL);
	
	
}

void handler_stp(int sig_num){
	printf("smash > signal SIGINT was sent to pid %d\n",getpid());
	if (!L_Fg_Cmd[0] || !fgPid) return;
	fgPid = 0;
	L_Fg_Cmd = '\0';
	pause();
}