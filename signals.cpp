// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"


void handler_int(int sig_num){
	printf("smash > signal SIGINT was sent to pid %d\n",getpid());
	if (!L_Fg_Cmd[0]) return;
	fflush(stdout);
	kill(Fg_pid, SIGKILL);
	
	
}

void handler_stp(int sig_num){
	printf("smash > signal SIGINT was sent to pid %d\n",getpid());
	if (!L_Fg_Cmd[0] || !Fg_pid) return;
	Fg_pid = 0;
	L_Fg_Cmd = '\0';
	pause();
}
