//		commands.c
//********************************************
#include "commands.h"
#include "signals.h"
#include <limits.h>

extern int fgPid; // global int which its default value of zero implies that no process is running in the foreground.
extern string fgCmd; // this represents the command of the foreground.
extern vector<Job> Vjobs; //a vector of Job class.
extern vector<string> hist;
//a vector that holds the history namely the past commands
// up to the current point.
extern char prvpwd[MAX_LINE_SIZE]; //this is a char[] which represents the previous command
// given by the user.

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(void* jobs, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = (char*)" \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
	}

	getcwd(pwd,sizeof(pwd));
	//now we are adding the command to the history
	hist.push_back(string(cmdString));
	if (hist.size() > MAX_HISTORY_SIZE) //implemtation of 'cyclic' vector
	{
		hist.erase(hist.begin());
	}
	// now we are getting rid of jobs that are already finished:
	for (vector<Job>::iterator iterator = Vjobs.begin(); iterator != Vjobs.end(); iterator++)
	{
		if (waitpid((*iterator).getpid(), NULL, WNOHANG)) // erasing after waiting on the job.
		{
			Vjobs.erase(iterator);
			iterator--;
		}
	}

/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		if (num_arg != 1)
		{
			illegal_cmd = TRUE;
		}
		else
		{
			//using chdir to change directory.
			if (!strcmp(args[1], "-"))
			{
				if (strcmp(prvpwd, ""))
				{
					if (chdir(prvpwd)!=-1) //cd successful
					{
						printf("%s\n",prvpwd);
						strcpy(prvpwd, pwd);
					}
					else
					{ //ERROR
						fprintf(stderr,"smash error: > ");
						fprintf(stderr,"\"%s\" - path not found\n",prvpwd);
						//the path is not found
					}
				}
			}
			else
			{
				if (chdir(args[1])!=-1) //LEGAL PATH
				{
					strcpy(prvpwd, pwd);
					strcpy(pwd, args[1]);
				}
				else
				{ //ILLEGAL PATH RECEIVED
					fprintf(stderr,"smash error: >\" %s\" - path not found\n",args[1]);
				}
			}
		}
	} 

	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if (num_arg != 0)
		{
			illegal_cmd = TRUE;
		}
		else
		{
			getcwd(pwd, sizeof(pwd)); //using getcwd to get current directory.
			printf("%s\n",pwd);
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
		if (num_arg != 0)
		{
			illegal_cmd = TRUE;
		}
		else
		{
			long int jobDuration=0; //(prevent possibly unitiated values warnings)
			long int curTime=0; //(prevent possibly unitiated values warnings)
			int i;
			for (i=0,curTime=time(NULL); i<Vjobs.size(); ++i)
			{
				printf("%d\n",i);
				jobDuration = curTime - Vjobs[i].getBeginningTime();
				printf("[%lu] %s: %d %lu secs",i+1,Vjobs[i].getcmd().c_str(),Vjobs[i].getpid(),jobDuration);
				if (Vjobs[i].isStopped()==true)
				{
					printf(" (Stopped)");
				}
				printf("\n");
			}
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if (num_arg != 0)
		{
			illegal_cmd = TRUE;
		}
		else
		{
			printf( "smash pid is %d\n",getpid());
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		if (num_arg > 1)
		{
			illegal_cmd = TRUE;
		}

		else
		{
			// we will find the right job
			// then take it to fg
			int Jindex =(num_arg)?atoi(args[0]-1):(Vjobs.size()-1);
			// assuming jindex is legal we can proceed:
			if (Jindex<Vjobs.size()&&Jindex>=0)
			{
				//insert to fgCmd the relevant job
				fgCmd = string(Vjobs[Jindex].getcmd());
				//print the fg Command
				printf("%s\n", fgCmd.c_str());
				//insert the right value to fgPid:
				fgPid = Vjobs[Jindex].getpid();
				//we are running in the foreground and waiting til the job is done:
				kill(fgPid, SIGCONT); //signal for fgPid to continue
				printf( "smash > signal SIGCONT was sent to pid %d\n",fgPid);
				Vjobs.erase(Vjobs.begin()+Jindex); //updating Vjobs to exclude Jindex job
				waitpid(fgPid, NULL, WUNTRACED); //wait until the fg job is done
				fgPid = 0; //update fgPid that there is no foreground job anymore
			}
			else // jindex is illegal
			{
				fprintf(stderr,"smash error: > \"%s\"\n", cmdString);
				return 1;
			}
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		if (num_arg > 1)
		{
			illegal_cmd = TRUE;
		}
		else
		{
			if (num_arg) // perform in the bg of the desired job according to the number.
			{
				if ( atoi(args[1]) <= Vjobs.size() && atoi(args[1]) > 0) //atoi(args[1]) stands for the number of command
				{
					int cmdNumber = atoi(args[1]);
					if (!(Vjobs[--cmdNumber].isStopped()))
					{
						printf("Job is already running in the background\n");
					}
					else // see whether the job is stopped, and adjust cmdNumber to Vjobs
					{
						printf("%s\n",Vjobs[cmdNumber].getcmd().c_str());
						int pid_number = Vjobs[cmdNumber].getpid();
						Vjobs[cmdNumber].setStopped(false);
						 // now we are signaling to continue the process
						kill(pid_number, SIGCONT);
						printf("smash > signal SIGCONT was sent to pid %d\n",pid_number);
					}
				}
				else
					// there was a problem
				{
					fprintf(stderr,"smash error: > \"%s\"\n",cmdString);
				}
			}
			else // bg without number
			{
				int lastStopped = INT_MIN;
				bool isAnyJobStopped = false;
				int i=0;
				// lets find the last stopped one in Vjobs:
				while (i<Vjobs.size())
				{
					if (i>lastStopped)
					{
						lastStopped = (Vjobs[i].isStopped())?i:lastStopped;
					}
					i++;
				}
				isAnyJobStopped=(lastStopped==INT_MIN)?true:isAnyJobStopped;
				if (isAnyJobStopped) // found stopped job
				{
					Vjobs[lastStopped].setStopped(false);
					printf("%s\n",Vjobs[lastStopped].getcmd().c_str());
					//performing the last stopped job
					kill(Vjobs[lastStopped].getpid(), SIGCONT); // signaling the job to continue
					printf("smash > signal SIGCONT was sent to pid %d\n",Vjobs[lastStopped].getpid());
				}
				else
				{
					// no stopped jobs in Vjobs.
					printf("There is no stopped job\n");
				}
			}
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if (num_arg > 1)
		{
			illegal_cmd = TRUE;
		}

		else
		{
			if (!num_arg)
			{
				exit(0);
			}

			else if (!strcmp(args[1],"kill"))
			{
				long int timesofar, timeinfivesec;
				for (std::size_t i=0; i<Vjobs.size(); ++i)
				{
					printf("[%lu] %s - Sending SIGTERM... ",i+1, Vjobs[i].getcmd().c_str());
					kill(Vjobs[i].getpid(), SIGTERM);
					timesofar=time(NULL);
					for(timeinfivesec = timesofar + 5;timesofar <= timeinfivesec;timesofar=time(NULL));
					//the following if occures
					// if the process has not finished therefore we are signaling SIGKILL to kill it.
					if (timesofar>timeinfivesec&&!waitpid(Vjobs[i].getpid(), NULL, WNOHANG))
						{
							printf( "(5 sec passed) Sending SIGKILL... ");
							kill(Vjobs[i].getpid(), SIGKILL);
						}
					printf( "Done.\n");
				}
				exit(0);
			}
			else
				illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{
		if (num_arg != 2) 
		{
			illegal_cmd = TRUE;
		}
		else 
		{
			//qutotation about mv:
			/*On success, zero is returned.  On error, -1 is returned, and errno is
       set appropriately.*/
			if (!rename(args[1], args[2])) // return of 0 indicates success.
			{
				printf("%s has been renamed to %s\n",args[1],args[2]);
			}
			else
			{
				fprintf(stderr,"rename has failed.\n");
			}

		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		if (num_arg != 0)
		{
			illegal_cmd = TRUE;
		}

		else
		{
			for (std::size_t i=0; i<hist.size()-1; ++i)
				printf("%s\n",hist[i].c_str());
			// explanation:
			// we exclude the last command which is always "history"
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if (num_arg != 2)
		{
			illegal_cmd = TRUE;
			fprintf(stderr,"smash error: > \"%s\"\n", cmdString);
			return 1;
		}

		else
		{
			int signal_number = abs(atoi(args[1]));
			int target_job = atoi(args[2]) - 1;
			// we can send the kill signal:
			if (!signal_number&&target_job>=0&&target_job<=Vjobs.size()){					   
				if (!kill((Vjobs[target_job]).getpid(), signal_number))
				// return of 0 from kill indicates success on sending.
				{
					string signal_types[33] = { "SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP","SIGABRT","SIGBUS","SIGFPE","SIGKILL","SIGUSR1",
					"SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM","SIGTERM","SIGSTKFLT","SIGCHLD","SIGCONT","SIGSTOP",
					"SIGTSTP","SIGTTIN","SIGTTOU","SIGURG","SIGXCPU","SIGXFSZ","SIGVTALRM","SIGPROF","SIGWINCH",
					"SIGIO","SIGPWR","SIGSYS","SIGRTMIN" };
					const char* signalType = signal_types[signal_number-1].c_str();
					printf("smash > signal %s was sent to pid %d\n",signalType, Vjobs[target_job].getpid());
					bool sigstopstop = (signalType == "SIGSTOP") || (signalType == "SIGTSTP");
					bool sigcont = signalType == "SIGCONT";
					
					if (sigstopstop) //we have to stop in this case:
					{
						Vjobs[target_job].setStopped(true);
					}
					else if (sigcont) //we have to continue
					{
						Vjobs[target_job].setStopped(false);
					}
					
				}
				else
				{
					fprintf(stderr,"smash error: > kill %s - cannot send signal\n",args[2]);
					return 1;
				}
			}
			else if (!signal_number)
			// this deals with signal_number that is illegal
			{
				fprintf(stderr,"smash error: > kill %s - cannot send signal\n",args[2]);
				return 1;
			}

			//check if job exists
			else // this deals with job that is out of the Jobs range (0 to the size).
			{
				fprintf(stderr,"smash error: > kill %s - job does not exist\n",args[2]);
				return 1;
			}
		}
	}

	/*************************************************/
	else 
	{
 		ExeExternal(args, cmdString);
		// if we got here,
		 //execute a command which is an external one.
	 	return 0;
	}

    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	// Add defaulting of sig handlers
	int pID;
	switch (pID = fork())
	{
	case 0: // the son
	{
		setpgrp(); //same group
		int val = execvp(args[0], args); //return of <0 indicates a problem.
		if (val < 0)
		{
			fprintf(stderr,"Error while executing external command %s\n",args[0]);
			//printf("smash >  signal SIGKILL was sent to pid %d\n",Vjobs[i].getpid());
			//fflush(stdout);
			kill(getpid(), SIGKILL); // we have to send a signal to kill the process.
		}
		return;
	}
	case -1: //fork is unsuccesful
	{
		printf("Error while executing external command %s\n",args[0]);
		return;
	}
	default: //performs command in the *foreground
	{
		fgCmd = string(cmdString); //update the fgCmd
		fgPid = pID; //here we are keeping fgPid updated (so it does not hold an incorrect value which can be snooped by another thread or process)
		waitpid(fgPid, NULL, WUNTRACED); // sleep until the soon finishes.
		fgPid = 0; // no foreground process is operating anymore.
		return;
	}
	}
 
}

//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize) //this is redundant according to the exercise
// not needed
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, char* cmdString)
{

	char* cmd;
	char* args[MAX_ARG];
	char* delimiters = (char*)" \t\n";

	if (lineSize[strlen(lineSize) - 2] == '&')
	{
		lineSize[strlen(lineSize) - 2] = '\0';
		cmd = strtok(lineSize, delimiters);
									
		   
		if (cmd == NULL)
			return 0;
		int i = 0;
		args[0] = cmd;
		for (i = 1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
		}

		int pid;
		switch (pid = fork())
		{
		case 0: //this is the son
		{
			//L_Fg_Cmd[0] = '\0';
			setpgrp(); // set the same group
			int val = execvp(args[0], args); // val indicates if there was a problem
			if (val < 0)
			{
				fprintf(stderr,"Error while executing external command %s\n",args[0]);
				//printf("smash >  signal SIGKILL was sent to pid %d\n",Vjobs[i].getpid());
				fflush(stdout);
				kill(getpid(), SIGKILL);
				// signaling SIGKILL to finish the current son.
			}
			return -1;
		}
		case -1:
		{
			// Fork unsucessful
			fprintf(stderr,"Error while executing external command %s\n",args[0]);
			return -1;
		}

		default: //running in the background
		{
			 //creating a job which its stopped is false, and using the current Time.
			 //using the first constructor of the job class:
			Job *addedJob = new Job(pid, string(cmdString));
			Vjobs.push_back(*addedJob);
			return 0;
		}
		}
	}
	return -1;
}