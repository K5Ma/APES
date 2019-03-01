#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <malloc.h> 
#include <stdbool.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 


/* Defines */


/* Global Varibales */
pthread_mutex_t lock;				//Used to lock or unlock important sections
volatile bool KILLx_x = false;		//This flag is set when the user sends a kill command, it will gracefully end our IPC
volatile int SigNum = 0;			//This will store the signal number sent by the user
volatile bool CPUMeas_Flag = false;	//This flag will be set to true when its time to take CPU measurment in pthread 2

/* Ptrhead argument structure */
typedef struct Pthread_ArgsStruct
{
    char* LogFile;					//Used to store the log filename to pass to pthread
	char* ReadFrom; 				//File name to read from (used by pthread1)
}Pthread_ArgsStruct;


/* This function was gotten from: 
 * https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c/8465083
 * It helps combine two strings in C. Slightly modified */
char* concat(const char *s1, const char *s2)
{
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	char *result = malloc(len1 + len2 + 1); //+1 for the null-terminator
	
	if(result == NULL)
	{
		printf("ERROR in concat(): malloc failed!\n");
		exit(0);
	}
	
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
	return result;
}


/* 
 * This function checks if the user wanted to kill demo :(
 */
bool KillCheck(uint8_t Thread, char* name)
{
	if(KILLx_x)
	{
		FILE *MyFileP;						//File discriptor
		struct timeval time;				//Declaring structure for time
		
		pthread_mutex_lock(&lock);			//Lock as we are accesing a shared file
		
		/* Check what thread first */
		switch(Thread)
		{
			case 1:
				/* This switch checks what signal was recieved, USR1 or USR2 */
				switch(SigNum)
				{
					case SIGUSR1:
						gettimeofday(&time, 0);		//Get current time and save it
						
						MyFileP = fopen(name, "a");
				
						fprintf(MyFileP, "#######################################\n");
						fprintf(MyFileP, "#         Child 1 Thread...           #\n");
						fprintf(MyFileP, "#                  was killed x_x     #\n");
						fprintf(MyFileP, "#   by the user sending a SIGUSR1...  #\n");
						fprintf(MyFileP, "#                                     #\n");
						fprintf(MyFileP, "#      Closing open file...           #\n");
						fprintf(MyFileP, "#      Signal value: %d                #\n", SigNum);
						fprintf(MyFileP, "#      Recvieved at: %ld.%ld          #\n", time.tv_sec, time.tv_usec);
						fprintf(MyFileP, "#######################################\n");
				
						/* Close the file */
						fclose(MyFileP);
						break;
						
					case SIGUSR2:
						gettimeofday(&time, 0);		//Get current time and save it
						
						MyFileP = fopen(name, "a");
				
						fprintf(MyFileP, "#######################################\n");
						fprintf(MyFileP, "#         Child 1 Thread...           #\n");
						fprintf(MyFileP, "#                  was killed x_x     #\n");
						fprintf(MyFileP, "#   by the user sending a SIGUSR2...  #\n");
						fprintf(MyFileP, "#                                     #\n");
						fprintf(MyFileP, "#      Closing open file...           #\n");
						fprintf(MyFileP, "#      Signal value: %d                #\n", SigNum);
						fprintf(MyFileP, "#      Recvieved at: %ld.%ld          #\n", time.tv_sec, time.tv_usec);
						fprintf(MyFileP, "#######################################\n");
				
						/* Close the file */
						fclose(MyFileP);
						break;
				}
				break;
				
			case 2:
				/* This switch checks what signal was recieved, USR1 or USR2 */
				switch(SigNum)
				{
					case SIGUSR1:
						gettimeofday(&time, 0);		//Get current time and save it
						
						MyFileP = fopen(name, "a");
				
						fprintf(MyFileP, "#######################################\n");
						fprintf(MyFileP, "#         Child 2 Thread...           #\n");
						fprintf(MyFileP, "#                  was killed x_x     #\n");
						fprintf(MyFileP, "#   by the user sending a SIGUSR1...  #\n");
						fprintf(MyFileP, "#                                     #\n");
						fprintf(MyFileP, "#      Closing open file...           #\n");
						fprintf(MyFileP, "#      Signal value: %d                #\n", SigNum);
						fprintf(MyFileP, "#      Recvieved at: %ld.%ld          #\n", time.tv_sec, time.tv_usec);
						fprintf(MyFileP, "#######################################\n");
				
						/* Close the file */
						fclose(MyFileP);
						break;
						
					case SIGUSR2:
						gettimeofday(&time, 0);		//Get current time and save it
						
						MyFileP = fopen(name, "a");
				
						fprintf(MyFileP, "#######################################\n");
						fprintf(MyFileP, "#         Child 2 Thread...           #\n");
						fprintf(MyFileP, "#                  was killed x_x     #\n");
						fprintf(MyFileP, "#   by the user sending a SIGUSR2...  #\n");
						fprintf(MyFileP, "#                                     #\n");
						fprintf(MyFileP, "#      Closing open file...           #\n");
						fprintf(MyFileP, "#      Signal value: %d                #\n", SigNum);
						fprintf(MyFileP, "#      Recvieved at: %ld.%ld          #\n", time.tv_sec, time.tv_usec);
						fprintf(MyFileP, "#######################################\n");
				
						/* Close the file */
						fclose(MyFileP);
						break;
				}
				break;
		}
		
		pthread_mutex_unlock(&lock);		//Unlock as we are done accesing the shared file
		KILLx_x = false;
		pthread_exit(0);
		return true;
	}
	
	return false;
}


/* 
 * This function is called at the begining of the thread to store its inital stuff in a file
 */
void LogFile_Init(uint8_t Thread, char* file)
{
	FILE *MyFileP;							//File discriptor
	struct timeval time;					//Declaring structure for time
			
	pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
	
	switch(Thread)
	{
		case 1:
			gettimeofday(&time, 0);			//Get current time and save it
			
			/* Modify the permissions of the file to be read/write and open the file (for writing) */
			MyFileP = fopen(file, "a");
			
			/* Get the ID of the calling thread */
			fprintf(MyFileP, "\n***** Init Child Thread 1 ID: %ld ******\n", pthread_self());
			
			/* Get and write process ID */
			fprintf(MyFileP, "Process ID: %d\n", getpid());
			
			/* Get the process ID of the parent of the calling process */
			fprintf(MyFileP, "Parent Process ID: %d\n", getppid());
			
			/* Syscall */
			fprintf(MyFileP, "syscall(SYS_gettid): %ld\n", syscall(SYS_gettid));
			
			/* Time */
			fprintf(MyFileP, "Current time: %ld.%ld\n", time.tv_sec, time.tv_usec);
			
			fprintf(MyFileP, "*********************************************************\n\n");
			
			/* Close the file */
			fclose(MyFileP);
			break;
			
		case 2:
			gettimeofday(&time, 0);			//Get current time and save it
		
			/* Modify the permissions of the file to be read/write and open the file (for writing) */
			MyFileP = fopen(file, "a");
			
			/* Get the ID of the calling thread */
			fprintf(MyFileP, "\n***** Init Child Thread 2 ID: %ld ******\n", pthread_self());
			
			/* Get and write process ID */
			fprintf(MyFileP, "Process ID: %d\n", getpid());
			
			/* Get the process ID of the parent of the calling process */
			fprintf(MyFileP, "Parent Process ID: %d\n", getppid());
			
			/* Syscall */
			fprintf(MyFileP, "syscall(SYS_gettid): %ld\n", syscall(SYS_gettid));
			
			/* Time */
			fprintf(MyFileP, "Current time: %ld.%ld\n", time.tv_sec, time.tv_usec);
			
			fprintf(MyFileP, "*********************************************************\n\n");
			
			/* Close the file */
			fclose(MyFileP);
			break;
	}
	
	pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
}

/* 
 * This function is called at the end of the thread 
 */
void LogFile_Exit(uint8_t Thread, char* file)
{
	FILE *MyFileP;							//File discriptor
	struct timeval time;					//Declaring structure for time
			
	pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
	gettimeofday(&time, 0);					//Get current time and save it
	
	switch(Thread)
	{
		case 1:
			MyFileP = fopen(file, "a");
			fprintf(MyFileP, "###########################################################\n");
			fprintf(MyFileP, "#  Child Thread 1 has completed its work and is exiting.  #\n");
			fprintf(MyFileP, "#  Exit time: %ld.%ld                             #\n", time.tv_sec, time.tv_usec);
			fprintf(MyFileP, "###########################################################\n");
			fclose(MyFileP);
			break;
			
		case 2:
			MyFileP = fopen(file, "a");
			fprintf(MyFileP, "###########################################################\n");
			fprintf(MyFileP, "#  Child Thread 2 has completed its work and is exiting.  #\n");
			fprintf(MyFileP, "#  Exit time: %ld.%ld                             #\n", time.tv_sec, time.tv_usec);
			fprintf(MyFileP, "###########################################################\n");
			fclose(MyFileP);
			break;
	}
	
	pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
}


/* 
 * This function is called when a thread wnats to log soemthing 
 */
void LogFile_Msg(uint8_t Thread, char* file, char* Message)
{
	FILE *MyFileP;							//File discriptor
	struct timeval time;					//Declaring structure for time
			
	pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
	
	gettimeofday(&time, 0);					//Get current time and save it
			
	switch(Thread)
	{
		case 1:
			MyFileP = fopen(file, "a");
			fprintf(MyFileP, "> [%ld.%ld] Child Thread 1: %s\n", time.tv_sec, time.tv_usec, Message);
			fclose(MyFileP);
			break;
			
		case 2:
			MyFileP = fopen(file, "a");
			fprintf(MyFileP, "> [%ld.%ld] Child Thread 2: %s\n", time.tv_sec, time.tv_usec, Message);
			fclose(MyFileP);
			break;
	}
	
	pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
}



/***************** LL ***********************/
/* My Node struct */
typedef struct Node
{
	char Alphabet;
	uint16_t Alphabet_Count;
	struct Node* next;
}Node;


/* 
 * This function will print the linked list
 */
 void PrintLinkedList(Node *head)
{
	while(head != NULL)
	{
		printf("%c: %u\n", head->Alphabet, head->Alphabet_Count);
		head = head->next;
	}
}

/* 
 * This function will check if an alphabet exists in the LL and increment its count
 */
bool AlphabetCheckInLL(Node *head, char Alpa)
{
	while(head != NULL)
	{
		if(head->Alphabet == Alpa)
		{
			head->Alphabet_Count++;
			//printf("Found %c in LL! Incrementing count: %u\n", head->Alphabet, head->Alphabet_Count);
			return true;
		}
		head = head->next;
	}
	
	return false;
}

/* 
 * This function will add an alphabet to the LL
 */
void AlphabetAddToLL(Node *head, char Alpa)
{
	while(head != NULL)
	{
		if(head->next == NULL)
		{
			struct Node* temp = NULL; 
			temp = (struct Node*)malloc(sizeof(struct Node));
			
			head->next = temp;
			temp->Alphabet = Alpa;
			temp->Alphabet_Count++;
			temp->next = NULL;
			//printf("Added %c to LL! Count: %u\n", temp->Alphabet, temp->Alphabet_Count);
			return;
		}
		head = head->next;
	}
}

/* 
 * This function print to log file alphabets less than 100
 */
void AlphabetPrintToLog(char* logfile, Node *head)
{
	FILE *MyFileP;						//File discriptor
	
	while(head != NULL)
	{
		if( head->Alphabet_Count < 100 )
		{
		//	printf("DEBUG: %c: %u \n", head->Alphabet, head->Alphabet_Count);
			pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
			MyFileP = fopen(logfile, "a");
			fprintf(MyFileP, "> Child Thread 1: %c: %u \n", head->Alphabet, head->Alphabet_Count);
			fclose(MyFileP);
			pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
		}
		head = head->next;
	}
}


/* 
 * This function will free our LL
 */
void FreeAlphabetLL(Node *head)
{
	struct Node* temp = NULL; 
	temp = (struct Node*)malloc(sizeof(struct Node));
	
	while(head != NULL)
	{
		temp = head;			//Set to current head
		head = head->next;		//Move head to the next avaible node
		free(temp);				//Free temp
	}
}

void CountAlphabet(char* logfile, char* file)
{
	LogFile_Msg(1, logfile, "Starting read from input text file...");
	
	FILE *MyFileP;										//File discriptor
	MyFileP = fopen(file, "r");							//Open file - read only
	
	KillCheck(1, logfile);
	
	/* Linked List */
	struct Node* head = NULL;
	struct Node* current = NULL; 
	head  = (struct Node*)malloc(sizeof(struct Node));
	current  = (struct Node*)malloc(sizeof(struct Node));
	
	/*Initialze the List Head to 'a' */
	head->Alphabet = 'a';				
	head->Alphabet_Count = 0;
	head->next = NULL;				
	
	/* Set current to head */
	current = head;
	
	/* Error check */
	if(MyFileP == NULL)
	{
		printf("ERROR in Child_Thread1: Could not open file to read from!\n");
		exit(0);										//Bigly error, could not open file to read from
	}
	
	KillCheck(1, logfile);
	
	LogFile_Msg(1, logfile, "Creating a LinkedList to store alphabets...");
	
	char char_read;										//Temp var to store got chars
	/* Loop until End Of File */
	while ( (char_read = fgetc(MyFileP) ) != EOF )
	{
		KillCheck(1, logfile);
		
		/* Check char is an alphabet */
		if( isalpha(char_read) )
		{
			/* Convert to lowercase */
			char_read = tolower(char_read);
			
			//DEEEEEEBUUUGG
		//	printf("GOT CHAR: %c!\n", char_read);
			
			/* Check if it exisits in LL */
			current = head;								//Reset current to head
			if( !AlphabetCheckInLL(current, char_read) )
			{
				current = head;							//Reset current to head
				/* If not, add it to LL */
				AlphabetAddToLL(current, char_read);
			}
		}
	}
	
	fclose(MyFileP);
	
	LogFile_Msg(1, logfile, "Done populating LinkedList!");
	LogFile_Msg(1, logfile, "Printing alphabets stored in LinkedList that have a count less than a 100:");
	
	KillCheck(1, logfile);
	
	current = head;										//Reset current to head
	AlphabetPrintToLog(logfile, current);
	
	KillCheck(1, logfile);
	
	LogFile_Msg(1, logfile, "Freeing all LinkedList nodes...");
	current = head;										//Reset current to head
	FreeAlphabetLL(current);
	
	LogFile_Msg(1, logfile, "Done freeing nodes!");
}


void * Child_Thread1(void * args)
{
	printf("**DEBUG** Thread 1 reached!\n");
	
	struct Pthread_ArgsStruct *Arguments = args;
	
	KillCheck(1, Arguments->LogFile);
	
	LogFile_Init(1, Arguments->LogFile);
	
	KillCheck(1, Arguments->LogFile);
	
	CountAlphabet(Arguments->LogFile, Arguments->ReadFrom);			//This funciton does all the heavy work
	
	KillCheck(1, Arguments->LogFile);
	
	LogFile_Exit(1, Arguments->LogFile);
	printf("**DEBUG** Exiting Thread 1...\n");
	
	return NULL;
}


void * Child_Thread2(void * args)
{
	printf("**DEBUG** Thread 2 reached!\n");
	
	struct Pthread_ArgsStruct *Arguments = args;
	
	LogFile_Init(2, Arguments->LogFile);
	
	FILE *MyFileP_CMD, *MyFileP_Log;					//File discriptors
	
	/* Path to source of cpu usage data */
	//char *CPUSource_Path = "cat /proc/stat | head -n 5";
	char *CPUSource_Path = "cat /proc/stat";
		
	/* This loop will keep Child Thread 2 alive */
	while(1)
	{
		KillCheck(2, Arguments->LogFile);
		
		/* Wait for the signal to take CPU measurment */
		while(!CPUMeas_Flag);
		printf("**DEBUG** Taking CPU measurment!\n");
		
		/* Create a pipe between the calling program and the executed 'cat' command process */
		MyFileP_CMD = popen(CPUSource_Path, "r");
		
		/* If file is opened: */
		if(MyFileP_CMD)
		{
			LogFile_Msg(2, Arguments->LogFile, "Capturing current CPU stats:");
			
			KillCheck(2, Arguments->LogFile);
			
			/* Loop until End Of File */
			char char_read;							//Temp var to store got chars
			pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
			
			MyFileP_Log = fopen(Arguments->LogFile, "a");
			
			while ( (char_read = fgetc(MyFileP_CMD) ) != EOF )
			{
				KillCheck(2, Arguments->LogFile);
				
				fprintf(MyFileP_Log, "%c", char_read);
			}
			
			fclose(MyFileP_Log);
			pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
			
			KillCheck(2, Arguments->LogFile);
			
			fclose(MyFileP_CMD);
		}
		
		CPUMeas_Flag = false;
	}
	
	LogFile_Exit(2, Arguments->LogFile);
	printf("**DEBUG** Exiting Thread 2...\n");
	
	return NULL;
}


void handler(int signum, siginfo_t *info, void* extra)
{	
	if( (SIGUSR1 == signum) || (SIGUSR2 == signum) )
	{
		KILLx_x = true;
		SigNum = signum;
	}

	else if(SIGVTALRM == signum)
	{
		CPUMeas_Flag = true;
	}
}



void Set_SigHandler()
{
	struct sigaction action;
	struct itimerval My_Timer;
	
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = handler;
	
	//action.sa_handler = &cpu_usage;
	
	/* Set timer interval to 100ms */
	My_Timer.it_interval.tv_sec = 0;
	My_Timer.it_interval.tv_usec = 100000;

	/* Set initial delay to 100ms */
	My_Timer.it_value.tv_sec = 0;
	My_Timer.it_value.tv_usec = 100000;
	
	
	printf("**DEBUG** SIGUSR1 = %d\n", SIGUSR1);
	printf("**DEBUG** SIGUSR2 = %d\n", SIGUSR2);
	//printf("SIGTERM = %d\n", SIGTERM);
	//printf("SIGINT = %d\n", SIGINT);

	if( sigaction(SIGUSR1, &action, NULL) == -1)
	{
		perror("SIGUSR1: sigaction");
		_exit(1);
	}
	
	if( sigaction(SIGUSR2, &action, NULL) == -1)
	{
		perror("SIGUSR2: sigaction");
		_exit(1);
	}
	
	if( sigaction(SIGVTALRM, &action, NULL) == -1)
	{
		perror("SIGUSR2: sigaction");
		_exit(1);
	}
	
	/* Start timer */
	setitimer(ITIMER_VIRTUAL, &My_Timer, NULL);
	
//	if( sigaction(SIGTERM, &action, NULL) == -1)
//	{
//		perror("SIGTERM: sigaction");
//		_exit(1);
//	}
//	
//	if( sigaction(SIGINT, &action, NULL) == -1)
//	{
//		perror("SIGINT: sigaction");
//		_exit(1);
//	}
}


int main(int argc, char** argv)
{
	/* Create two pthreads */
	pthread_t Thread1, Thread2;
	
	/* Set-up Signal Handler */
	Set_SigHandler();
	
	/* Error handling */
	if(argc != 3)
	{
		printf("ERROR: Please enter a name for the output log file and a txt file name to read from!\n");
		return -1;
	}
	
	/* Store filename */
	char* FilePath = concat("./", argv[1]);
	FilePath = concat(FilePath, ".txt");
	printf("**DEBUG** Got filename: '%s'!\n", FilePath);
	
	/* Store the filename to be read */
	char* FilePath_Read = concat("./", argv[2]);
	printf("**DEBUG** Got filename to read from: '%s'!\n", FilePath_Read);
	
	/* Store filepath to pass to pthreads */
	struct Pthread_ArgsStruct args;
	args.LogFile = FilePath;
	args.ReadFrom = FilePath_Read; 
	
	
	printf("**DEBUG** Starting Problem 1...\n");
	FILE *MyFileP;														//File discriptor
	MyFileP = fopen(FilePath, "w");
	fprintf(MyFileP, "Starting Problem 1...\n");
	fclose(MyFileP);

	pthread_create(&Thread1, NULL, &Child_Thread1, (void *)&args);
	pthread_create(&Thread2, NULL, &Child_Thread2, (void *)&args);
	
	/* Wait for pthreads to finish */
	pthread_join(Thread1, NULL);
	pthread_join(Thread2, NULL);
	
	/* Log exit */
	struct timeval time;				//Declaring structure for time
	gettimeofday(&time, 0);				//Get current time and save it
	MyFileP = fopen(FilePath, "a");
	fprintf(MyFileP, "\n***********************************************\n");
	fprintf(MyFileP, "* Child 1 and Child 2 have finished.          *\n");
	fprintf(MyFileP, "*        Exiting program...                   *\n");
	fprintf(MyFileP, "*        Exit time: %ld.%ld               *\n", time.tv_sec, time.tv_usec);
	fprintf(MyFileP, "***********************************************\n");
	fclose(MyFileP);
	
	printf("**DEBUG** Done!\n");
	
	return 0;
}
