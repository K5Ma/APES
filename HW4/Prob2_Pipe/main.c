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
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <malloc.h> 
#include <stdbool.h> 


/* Defines */
//int Thread1_Pipe[2];			//Used to store two ends of first pipe 
//int Thread2_Pipe[2];			//Used to store two ends of second pipe 
//

/* Global Varibales */
FILE *MyFileP;						//File pointer
pthread_mutex_t lock;				//Used to lock or unlock important sections
volatile bool KILLx_x = false;		//This flag is set when the user sends a kill command, it will gracefully end our IPC
volatile int SigNum = 0;			//This will store the signal number sent by the user


/* My message structure */
typedef struct MsgStruct 
{
	char Msg[50];
	uint8_t Len;
	bool LED_Cntrl;
}MsgStruct;


/* Ptrhead argument structure */
typedef struct Pthread_ArgsStruct
{
    int Thread1_Pipe[2];			//Used to store two ends of first pipe 
    int Thread2_Pipe[2];			//Used to store two ends of second pipe 
}Pthread_ArgsStruct;


/* 
 * This function checks if the user wanted to kill demo :(
 */
void KillCheck(Pthread_ArgsStruct * pipes)
{
	if(KILLx_x)
	{
		/* File pointer */
		FILE *MyFileP;
		/* Declaring structure for time */
		struct timeval time;
	
		pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
		
		/* Get current time and save it */
		gettimeofday(&time, 0);
			
		/* Modify the permissions of the file to be read/write and open the file (for writing) */
		MyFileP = fopen("./MyLogFile.txt", "a");
		
		fprintf(MyFileP, "#######################################\n");
		fprintf(MyFileP, "#        Process...                   #\n");
		fprintf(MyFileP, "#                   was killed x_x    #\n");
		fprintf(MyFileP, "#      by the user......              #\n");
		fprintf(MyFileP, "#                                     #\n");
		fprintf(MyFileP, "#      Closing all pipes...           #\n");
		fprintf(MyFileP, "#      Signal value: %d                #\n", SigNum);
		fprintf(MyFileP, "#      Recvieved at: %ld.%ld          #\n", time.tv_sec, time.tv_usec);
		fprintf(MyFileP, "#######################################\n");
		
		/* Close the file */
		fclose(MyFileP);
		
		close(pipes->Thread1_Pipe[0]);
		close(pipes->Thread1_Pipe[1]);
		close(pipes->Thread2_Pipe[0]);
		close(pipes->Thread2_Pipe[1]);
		
		pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
		
		exit(0);
	}
}


/* 
 * This functoin is called at the begining of the thread to store its inital stuff in a file
 */
void LogFile_Init()
{
	/* File pointer */
	FILE *MyFileP;
	
	pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
	
	/* Modify the permissions of the file to be read/write and open the file (for writing) */
	MyFileP = fopen("./MyLogFile.txt", "a");
	
	/* Get the ID of the calling thread */
	fprintf(MyFileP, "***** Init Thread ID: %ld ******\n", pthread_self());
	
	/* Get and write process ID */
	fprintf(MyFileP, "Process ID: %d\n", getpid());
	
	/* Get the process ID of the parent of the calling process */
	fprintf(MyFileP, "Parent Process ID: %d\n", getppid());
	
	/* Syscall */
	fprintf(MyFileP, "syscall(SYS_gettid): %ld\n", syscall(SYS_gettid));
	
	fprintf(MyFileP, "***************************************\n");
	
	/* Close the file */
	fclose(MyFileP);
	
	pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
}

/* 
 * This functon is used to log any messages sent, first parameter determines which thread is sending (1 or 2)
 */
void LogFile_MsgSend(uint8_t Thread, MsgStruct* Msg)
{
	//Declaring structure for time
	struct timeval time;
	
	/* File pointer */
	FILE *MyFileP;
	
	pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
	
	switch(Thread)
	{
		case 1:
			/* Modify the permissions of the file to be read/write and open the file (for writing) */
			MyFileP = fopen("./MyLogFile.txt", "a");
			
			/* Get current time and save it */
			gettimeofday(&time, 0);
			
			/* Log msg */
			fprintf(MyFileP, "[%ld.%ld] Thread 1 - Sending msg:\n", time.tv_sec, time.tv_usec);
			fprintf(MyFileP, "                 |---> Contents: %s\n", Msg->Msg);
			fprintf(MyFileP, "                 |---> Length: %u\n", Msg->Len);
			fprintf(MyFileP, "                 L---> LED control: %d\n", Msg->LED_Cntrl);
	
			/* Close the file */
			fclose(MyFileP);
			break;
			
		case 2:
			/* Modify the permissions of the file to be read/write and open the file (for writing) */
			MyFileP = fopen("./MyLogFile.txt", "a");
			
			/* Get current time and save it */
			gettimeofday(&time, 0);
			
			/* Log msg */
			fprintf(MyFileP, "[%ld.%ld] Thread 2 - Sending msg:\n", time.tv_sec, time.tv_usec);
			fprintf(MyFileP, "                 |---> Contents: %s\n", Msg->Msg);
			fprintf(MyFileP, "                 |---> Length: %u\n", Msg->Len);
			fprintf(MyFileP, "                 L---> LED control: %d\n", Msg->LED_Cntrl);
	
			/* Close the file */
			fclose(MyFileP);
			break;
	}
	pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
}


/* 
 * This functon is used to log any recieved messages, first parameter determines which thread is receiving (1 or 2)
 */
void LogFile_MsgReceived(uint8_t Thread, MsgStruct* Msg)
{
	//Declaring structure for time
	struct timeval time;

	/* File pointer */
	FILE *MyFileP;
	
	pthread_mutex_lock(&lock);				//Lock as we are accesing a shared file
	
	switch(Thread)
	{
		case 1:
			/* Modify the permissions of the file to be read/write and open the file (for writing) */
			MyFileP = fopen("./MyLogFile.txt", "a");
			
			/* Get current time and save it */
			gettimeofday(&time, 0);
			
			/* Log msg */
			fprintf(MyFileP, "[%ld.%ld] Thread 1 - Received msg from Thread 2:\n", time.tv_sec, time.tv_usec);
			fprintf(MyFileP, "                 |---> Contents: %s\n", Msg->Msg);
			fprintf(MyFileP, "                 |---> Length: %u\n", Msg->Len);
			fprintf(MyFileP, "                 L---> LED control: %d\n", Msg->LED_Cntrl);
	
			/* Close the file */
			fclose(MyFileP);
			break;
			
		case 2:
			/* Modify the permissions of the file to be read/write and open the file (for writing) */
			MyFileP = fopen("./MyLogFile.txt", "a");
			
			/* Get current time and save it */
			gettimeofday(&time, 0);
			
			/* Log msg */
			fprintf(MyFileP, "[%ld.%ld] Thread 2 - Received msg from Thread 1:\n", time.tv_sec, time.tv_usec);
			fprintf(MyFileP, "                 |---> Contents: %s\n", Msg->Msg);
			fprintf(MyFileP, "                 |---> Length: %u\n", Msg->Len);
			fprintf(MyFileP, "                 L---> LED control: %d\n", Msg->LED_Cntrl);
	
			/* Close the file */
			fclose(MyFileP);
			break;
	}
	pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
}



void * Thread1Pipe(void * args)
{
	printf("**DEBUG** Thread 1 reached!\n");
	
	struct Pthread_ArgsStruct *Arguments = args;
	
	LogFile_Init();
	
	
	if( pipe(Arguments->Thread1_Pipe) < 0) 
	{
		printf("**DEBUG** ERROR: Thread1 - Pipe Failed\n");
		return NULL; 
	}
	
	KillCheck(Arguments);
	
	/* Create a msg struct to send msgs */
	MsgStruct MsgP1;

	/* First, send 10 messages to the Pipe to be read by Thread2 */
	strcpy(MsgP1.Msg, "This is");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "a sequence");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "of 10 msgs");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "sent by pthread1");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "to pthread 2");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "which will");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "be recieved");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "very");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "very");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(Arguments);
	
	strcpy(MsgP1.Msg, "soon");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	write(Arguments->Thread1_Pipe[1], &MsgP1, sizeof(MsgStruct));
	LogFile_MsgSend(1, &MsgP1);
	
	/* Wait until Thread2 has its pipe ready to be read */
	sleep(2);
	
	KillCheck(Arguments);
	
	/* Next, read from Thread2 Pipe */
	MsgStruct MsgRecv;		//Stores the received message
	int NumBytes;			//Stores the number of bytes
	uint8_t Received_Count = 0;
	while ( (NumBytes = read(Arguments->Thread2_Pipe[0], &MsgRecv, sizeof(MsgStruct))) > 0 )
	{
		LogFile_MsgReceived(1, &MsgRecv);
		Received_Count++;
		
		KillCheck(Arguments);
		
		if(Received_Count == 10)
		{
			break;
		}
	}
	
	KillCheck(Arguments);
	
	/* Close Thread1 Pipe */
	close(Arguments->Thread2_Pipe[1]);
	close(Arguments->Thread2_Pipe[0]);
	
	printf("**DEBUG** Exiting Thread 1...\n");
	
	return NULL;
}


void * Thread2Pipe(void * args)
{
	printf("**DEBUG** Thread 2 reached!\n");
	
	struct Pthread_ArgsStruct *Arguments = args;
	
	LogFile_Init();
	
	if( pipe(Arguments->Thread2_Pipe) < 0) 
	{
		printf("**DEBUG** ERROR: Thread2 - Pipe Failed\n");
		return NULL; 
	}
	
	KillCheck(Arguments);
	
	/* First, read from Thread1 Pipe */
	MsgStruct MsgRecv;		//Stores the received message
	int NumBytes;			//Stores the number of bytes
	uint8_t Received_Count = 0;
	while ( (NumBytes = read(Arguments->Thread1_Pipe[0], &MsgRecv, sizeof(MsgStruct))) > 0 )
	{
		LogFile_MsgReceived(2, &MsgRecv);
		Received_Count++;
		
		KillCheck(Arguments);
		
		if(Received_Count == 10)
		{
			break;
		}
	}
	/* Close Thread1 Pipe */
	close(Arguments->Thread1_Pipe[1]);
	close(Arguments->Thread1_Pipe[0]);
	
	
	/* Next, send back 10 messages through Thread2 Pipe */
	/* Create a msg struct to send msgs */
	MsgStruct MsgP2;

	/* Send 10 messages to the queue */
	strcpy(MsgP2.Msg, "This is");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "a sequence");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "of msgs");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "sent by pthread 2");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "to pthread 1");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "it is");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "very cool");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "how this all");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "works");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(Arguments);
	
	strcpy(MsgP2.Msg, "noice");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	write(Arguments->Thread2_Pipe[1], &MsgP2, sizeof(MsgStruct));
	LogFile_MsgSend(2, &MsgP2);
	
	printf("**DEBUG** Exiting Thread 2...\n");
	
	return NULL;
}


void handler(int signum, siginfo_t *info, void* extra)
{
	KILLx_x = true;
	SigNum = signum;
}



void Set_SigHandler()
{
	struct sigaction action;
	
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = handler;
	
	//printf("SIGTERM = %d\n", SIGTERM);
	//printf("SIGUSR1 = %d\n", SIGUSR1);
	//printf("SIGINT = %d\n", SIGINT);
	
	if( sigaction(SIGTERM, &action, NULL) == -1)
	{
		perror("SIGTERM: sigaction");
		_exit(1);
	}
	
	if( sigaction(SIGINT, &action, NULL) == -1)
	{
		perror("SIGINT: sigaction");
		_exit(1);
	}
}


int main(int argc, char** argv)
{
	/* Create two pthreads */
	pthread_t Thread1, Thread2;
	
	/* Set-up Signal Handler */
	Set_SigHandler();
	
	FILE *MyFileP;
	MyFileP = fopen("./MyLogFile.txt", "a");
	fprintf(MyFileP, "Starting Pipe IPC Demo...\n");
	fclose(MyFileP);
	
	struct Pthread_ArgsStruct args;
	
	pthread_create(&Thread1, NULL, &Thread1Pipe, (void *)&args);
	sleep(1);
	pthread_create(&Thread2, NULL, &Thread2Pipe, (void *)&args);
	
	/* Wait for pthreads to finish */
	pthread_join(Thread1, NULL);
	pthread_join(Thread2, NULL);
	
	printf("Done!\n");
	
	return 0;
}
