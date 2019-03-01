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
#define THREAD_1_QUEUE			"/THREAD1_POSIX_Q"		//One POSIX queue for pthread1
#define THREAD_2_QUEUE			"/THREAD2_POSIX_Q"		//One POSIX queue for pthread2
#define MAX_SIZE_Q				1024


/* Global Varibales */
pthread_mutex_t lock;	//Used to lock or unlock important sections
volatile bool KILLx_x = false;		//This flag is set when the user sends a kill command, it will gracefully end our IPC
volatile int SigNum = 0;			//This will store the signal number sent by the user


/* My message structure */
typedef struct MsgStruct 
{
	char Msg[50];
	uint8_t Len;
	bool LED_Cntrl;
}MsgStruct;



/* 
 * This function checks if the user wanted to kill demo :(
 */
void KillCheck(mqd_t mq)
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
		fprintf(MyFileP, "#      Closing & unlinking all queues #\n");
		fprintf(MyFileP, "#      Signal value: %d                #\n", SigNum);
		fprintf(MyFileP, "#      Recvieved at: %ld.%ld          #\n", time.tv_sec, time.tv_usec);
		fprintf(MyFileP, "#######################################\n");
		
		/* Close the file */
		fclose(MyFileP);
		
		mq_close(mq);
		mq_unlink(THREAD_1_QUEUE);
		mq_unlink(THREAD_2_QUEUE);
		
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




void * Thread1_queue(void * args)
{
	printf("**DEBUG** Thread 1 reached!\n");
	
	LogFile_Init();
	
	mqd_t mq;						//Message queue descriptor
	ssize_t Msg_Bytes;				//Stores the number of bytes recivied
	
	char buffer[MAX_SIZE_Q + 1];	//Set the max buffer size
	
	/* Initialize the queue attributes */
	struct mq_attr attr;
	attr.mq_flags = 0;				//message queue flags
	attr.mq_maxmsg = 10;			//maximum number of messages
	attr.mq_msgsize = MAX_SIZE_Q;	//maximum message size
	attr.mq_curmsgs = 0;			//number of messages currently queued
	
	/* Open the Thread1 queue to store pointers into it - Write only */
	mq = mq_open(THREAD_1_QUEUE, O_CREAT | O_WRONLY | O_NONBLOCK, 0666, &attr);
	
	KillCheck(mq);
	
	/* Create a msg struct to send msgs */
	MsgStruct MsgP1;

	/* Send 10 messages to the queue */
	strcpy(MsgP1.Msg, "This is");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "a sequence");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "of 10 msgs");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "sent by pthread1");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "to pthread 2");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = false; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "which will");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "be recieved");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "very");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "very");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	strcpy(MsgP1.Msg, "soon");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true; 
	mq_send(mq, &MsgP1, MAX_SIZE_Q, 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck(mq);
	
	/* Close Thread1 queue */
	mq_close(mq);
	
	KillCheck(mq);
	
	/* Open the Thread2 queue - Read only */
	mq = mq_open(THREAD_2_QUEUE, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
	
	KillCheck(mq);
	
	/* Get the messages from the other thread */
	MsgStruct MsgRecv;
	uint8_t Recieved_count = 0;
	while(Recieved_count < 10)
	{
		Msg_Bytes = mq_receive(mq, &MsgRecv, MAX_SIZE_Q, NULL);
		
		KillCheck(mq);
		
        if(Msg_Bytes >= 0)
		{
			LogFile_MsgReceived(1, &MsgRecv);
			Recieved_count++;
			KillCheck(mq);
		}
    }
	/* Close Thread2 queue */
	mq_close(mq);

	/* Cleanup after being done */
	mq_unlink(THREAD_1_QUEUE);
	
	printf("**DEBUG** Exiting Thread 1...\n");
	
	return NULL;
}


void * Thread2_queue(void * args)
{
	printf("**DEBUG** Thread 2 reached!\n");
	
	LogFile_Init(); 
	
	mqd_t mq;						//Message queue descriptor
	ssize_t Msg_Bytes;				//Stores the number of bytes recivied
	
	char buffer[MAX_SIZE_Q + 1];	//Set the max buffer size
	
	/* Initialize the queue attributes */
	struct mq_attr attr;
	attr.mq_flags = 0;				//message queue flags
    attr.mq_maxmsg = 10;			//maximum number of messages
    attr.mq_msgsize = MAX_SIZE_Q;	//maximum message size
    attr.mq_curmsgs = 0;			//number of messages currently queued
	
	/* Open the Thread2 queue to store pointers into it - Write only */
	mq = mq_open(THREAD_2_QUEUE, O_CREAT | O_WRONLY | O_NONBLOCK, 0666, &attr);
	
	KillCheck(mq);
	
	/* Create a msg struct to send msgs */
	MsgStruct MsgP2;

	/* Send 10 messages to the queue */
	strcpy(MsgP2.Msg, "This is");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "a sequence");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "of msgs");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "sent by pthread 2");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "to pthread 1");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "it is");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "very cool");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "how this all");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "works");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	strcpy(MsgP2.Msg, "noice");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = false; 
	mq_send(mq, &MsgP2, MAX_SIZE_Q, 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck(mq);
	
	/* Close Thread2 queue */
	mq_close(mq);
	
	KillCheck(mq);
	
	/* Open the Thread1 queue - Read only */
	mq = mq_open(THREAD_1_QUEUE, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
	/* Recieve 10 messages from the queue */
	MsgStruct MsgRecv;
	uint8_t Recieved_count = 0;
	while(Recieved_count < 10)
	{
		Msg_Bytes = mq_receive(mq, &MsgRecv, MAX_SIZE_Q, NULL);
		KillCheck(mq);
		
        if(Msg_Bytes >= 0)
		{
			LogFile_MsgReceived(2, &MsgRecv);
			Recieved_count++;
			KillCheck(mq);
		}
    }
	
	/* Close Thread1 queue */
	mq_close(mq);
	
	/* Cleanup after being done */
	mq_unlink(THREAD_2_QUEUE);
	
	
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
	fprintf(MyFileP, "Starting POSIX Queue IPC Demo...\n");
	fclose(MyFileP);
	
	
	pthread_create(&Thread1, NULL, &Thread1_queue, NULL);
	pthread_create(&Thread2, NULL, &Thread2_queue, NULL);
	
	/* Wait for pthreads to finish */
	pthread_join(Thread1, NULL);
	pthread_join(Thread2, NULL);
	
	printf("Done!\n");
	
	return 0;
}
