//		commands.c
//********************************************
#include "commands.h"
#include "signals.h"
#include <limits.h>
char prvpwd[MAX_LINE_SIZE];
vector<Job> Vjobs;
vector<string> hist;
int fgPid = 0;
string fgCmd;
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
		if (waitpid((*iterator).getPid(), NULL, WNOHANG))
		{
			Vjobs.erase(iterator);
			iterator--;
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
				if (strcmp(prvpwd, "")) // if the previous path is not empty
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
	/*else if (!strcmp(cmd, "mkdir")) NOT NEEDED IN THE EXERCISE
	{
 		
	}*/
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
		if (num_arg != 0)
		{
			illegal_cmd = TRUE;
		}
		else
		{
			long int curTime, jobDuration;
			curTime=time(NULL);
			for (std::size_t i=0; i<Vjobs.size(); ++i)
			{
				jobDuration = curTime - Vjobs[i].getBeginningTime();
				printf("[%lu] %s: %d %lu secs",i+1,Vjobs[i].getCmd().c_str(),Vjobs[i].getPid(),jobDuration);
				if (Vjobs[i].isStopped()==true)
				{
					printf(" (Stopped)"); // check if the process was suspended
				}
				printf("\n");
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
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		
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
					printf("[%lu] %s - Sending SIGTERM... ",i+1, Vjobs[i].getCmd().c_str());
					kill(Vjobs[i].getPid(), SIGTERM);
					timesofar=time(NULL);
					timeinfivesec = timesofar + 5;
					for(;timesofar <= timeinfivesec;timesofar=time(NULL));
					if (timesofar>timeinfivesec&&!waitpid(Vjobs[i].getPid(), NULL, WNOHANG))
						{
							printf( "(5 sec passed) Sending SIGKILL... ");
							kill(Vjobs[i].getPid(), SIGKILL); //sending SIGKILL to process that is not ending
						}
					printf( "Done.\n");
				}
				exit(0);
			}
			else
				illegal_cmd = TRUE;
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
			if (rename(args[1], args[2]) != 0)
			{
				fprintf(stderr,"rename has failed.\n");
			}
			else 
			{
				printf("%s has been renamed to %s\n",args[1],args[2]);
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
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
				if (num_arg != 2)
		{
			illegal_cmd = TRUE;
		}
		else
		{
			// to be continued
		}
	}
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		fprintf(stderr,"smash error: > \"%s\"\n", cmdString);
		return 1;
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
	int pID;
	switch (pID = fork())
	{
	case -1: //fork is unsuccesful
	{
		printf("Error while executing external command %s\n",args[0]);
		return;
	}
  
	case 0:
	{
		// Child Process
		setpgrp();
		int val = execvp(args[0], args); //return of <0 indicates a problem.
		if (val < 0)
		{
			fprintf(stderr,"Error while executing external command %s\n",args[0]);
			kill(getpid(), SIGKILL);
		}
		return;
	}

	default: //performs command in the *foreground
		fgPid = pID;
		fgCmd = string(cmdString);
		waitpid(pID, NULL, WUNTRACED); //Wait for son process to finish
		fgPid = 0;
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
int ExeComp(char* lineSize) //I think THIS IS NOT NEEDED (according to the instructions)
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

		int pID;
		switch (pID = fork())
		{
		case -1:
		{
			// Fork unsucessful
			fprintf(stderr,"Error while executing external command %s\n",args[0]);
			return -1;
		}

		case 0:
		{
			//Child Process
			setpgrp();
			int val = execvp(args[0], args); // val indicates if there was a problem
			if (val < 0)
			{
				fprintf(stderr,"Error while executing external command %s\n",args[0]);
				kill(getpid(), SIGKILL);
			}
			return -1;
		}

		default: //running in the background
		{
			long int timeNow=time(NULL);
			Job* newJob = new Job(pID, string(cmdString), /*not stopped*/false, timeNow);
			Vjobs.push_back(*newJob);
			return 0;
		}
		}
	}
	return -1;
}



