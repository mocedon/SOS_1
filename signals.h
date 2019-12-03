#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

//SIGINT
void handler_int(int sig_num);

//SIGSTP
void handler_stp(int sig_num);

#endif

