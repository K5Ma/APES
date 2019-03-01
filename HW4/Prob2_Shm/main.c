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
#include <sys/shm.h>
#include <sys/mman.h>


/* Global Varibales */
pthread_mutex_t lock;	//Used to lock or unlock important sections
volatile bool KILLx_x = false;		//This flag is set when the user sends a kill command, it will gracefully end our IPC
volatile int SigNum = 0;			//This will store the signal number sent by the user


/* 
 * This functoin checks if the user wanted to kill demo :(
 */
void KillCheck(char *name)
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
		fprintf(MyFileP, "#      Unlinking Shared memory...     #\n");
		fprintf(MyFileP, "#                                     #\n");
		fprintf(MyFileP, "#      Signal value: %d                #\n", SigNum);
		fprintf(MyFileP, "#      Recvieved at: %ld.%ld          #\n", time.tv_sec, time.tv_usec);
		fprintf(MyFileP, "#######################################\n");
		
		/* Close the file */
		fclose(MyFileP);
		
		shm_unlink(name);
		
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
void LogFile_MsgSend(uint8_t Thread, char* Msg)
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
			fprintf(MyFileP, "[%ld.%ld] Thread 1 - Sending msg: %s\n", time.tv_sec, time.tv_usec, Msg);
	
			/* Close the file */
			fclose(MyFileP);
			break;
			
		case 2:
			/* Modify the permissions of the file to be read/write and open the file (for writing) */
			MyFileP = fopen("./MyLogFile.txt", "a");
			
			/* Get current time and save it */
			gettimeofday(&time, 0);
			
			/* Log msg */
			fprintf(MyFileP, "[%ld.%ld] Thread 2 - Sending msg: %s\n", time.tv_sec, time.tv_usec, Msg);

			/* Close the file */
			fclose(MyFileP);
			break;
	}
	pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
}


/* 
 * This functon is used to log any recieved messages, first parameter determines which thread is receiving (1 or 2)
 */
void LogFile_MsgReceived(uint8_t Thread, char* Msg)
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
			fprintf(MyFileP, "[%ld.%ld] Thread 1 - Received msg from Thread 2: %s\n", time.tv_sec, time.tv_usec, Msg);
	
			/* Close the file */
			fclose(MyFileP);
			break;
			
		case 2:
			/* Modify the permissions of the file to be read/write and open the file (for writing) */
			MyFileP = fopen("./MyLogFile.txt", "a");
			
			/* Get current time and save it */
			gettimeofday(&time, 0);
			
			/* Log msg */
			fprintf(MyFileP, "[%ld.%ld] Thread 2 - Received msg from Thread 1: %s\n", time.tv_sec, time.tv_usec, Msg);
	
			/* Close the file */
			fclose(MyFileP);
			break;
	}
	pthread_mutex_unlock(&lock);			//Unlock as we are done accesing the shared file
}




void * Thread1_SharedMem(void * args)
{
	printf("**DEBUG** Thread 1 reached!\n");
	
	LogFile_Init();
	
	/* Create 10 msgs to send */
	const char* msg1 = "This is a ";
	const char* msg2 = "sequence of 10 ";
	const char* msg3 = "messages sent from ";
	const char* msg4 = "pthread1 to pthread2 ";
	const char* msg5 = "using shared memory.\n";
	const char* msg6 = "The rest are LED controls:\n";
	const char* msg7 = "LED Control: ON\n";
	const char* msg8 = "LED Control: OFF\n";
	const char* msg9 = "LED Control: OFF\n";
	const char* msg10 = "LED Control: ON\n";
	
	/* size in bytes of the shared memory object */
	const int SIZE = 4096; 
	
	/* Name of the shared memory object */
	const char* name = "/SharedMem_P1";
	
	/* Shared memory file discriptor */
	int shm_fd; 
	
	/* Pointer to the shared memory object */
	void *ptr; 
	
	/* Open the shared memory object */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	
	KillCheck(name);
	
	/* Configure the size of shared memory object */
	ftruncate(shm_fd, SIZE);
	
	KillCheck(name);
	
	/* Memory mapping the shared object */
	ptr = mmap(NULL, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	
	KillCheck(name);
	
	/* Write the 10 msgs to shared memory space */
	sprintf(ptr, "%s", msg1);
	LogFile_MsgSend(1, msg1);
	ptr += strlen(msg1);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg2);
	LogFile_MsgSend(1, msg2);
	ptr += strlen(msg2);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg3);
	LogFile_MsgSend(1, msg3);
	ptr += strlen(msg3);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg4);
	LogFile_MsgSend(1, msg4);
	ptr += strlen(msg4);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg5);
	LogFile_MsgSend(1, msg5);
	ptr += strlen(msg5);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg6);
	LogFile_MsgSend(1, msg6);
	ptr += strlen(msg6);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg7);
	LogFile_MsgSend(1, msg7);
	ptr += strlen(msg7);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg8);
	LogFile_MsgSend(1, msg8);
	ptr += strlen(msg8);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg9);
	LogFile_MsgSend(1, msg9);
	ptr += strlen(msg9);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg10);
	LogFile_MsgSend(1, msg10);
	ptr += strlen(msg10);
	
	KillCheck(name);
	
	/* Wait for pthread2 to write to its shared memory */
	sleep(3);
	
	/* size in bytes of the shared memory object */
	const int SIZE_R = 4096; 
	
	/* Name of the shared memory object */
	const char* name_R = "/SharedMem_P2";
	
	/* Shared memory file discriptor */
	int shm_fd_R; 
	
	/* Pointer to the shared memory object */
	void *ptr_R; 
	
	/* Open the shared memory object */
	shm_fd_R = shm_open(name_R, O_RDONLY, 0666);
	
	KillCheck(name_R);
	
	/* Memory mapping the shared object */
	ptr_R = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, shm_fd_R, 0);
	
	KillCheck(name_R);
	
	/* Reading 10 msgs from shared memory space */
	LogFile_MsgReceived(1, (char *)ptr_R);
	
	KillCheck(name_R);
	
	/* Remove link to share object */
	shm_unlink(name_R);
	
	printf("**DEBUG** Exiting Thread 1...\n");
	
	return NULL;
}


void * Thread2_SharedMem(void * args)
{
	printf("**DEBUG** Thread 2 reached!\n");
	
	LogFile_Init(); 
	
	/* Create 10 msgs to send */
	const char* msg1 = "This is a ";
	const char* msg2 = "sequence of 10 ";
	const char* msg3 = "messages sent from ";
	const char* msg4 = "pthread2 to pthread1 ";
	const char* msg5 = "using shared memory.\n";
	const char* msg6 = "The rest are LED controls:\n";
	const char* msg7 = "LED Control: OFF\n";
	const char* msg8 = "LED Control: OFF\n";
	const char* msg9 = "LED Control: ON\n";
	const char* msg10 = "LED Control: ON\n";
	
	/* size in bytes of the shared memory object */
	const int SIZE = 4096; 
	
	/* Name of the shared memory object */
	const char* name = "/SharedMem_P2";
	
	/* Shared memory file discriptor */
	int shm_fd; 
	
	/* Pointer to the shared memory object */
	void *ptr; 
	
	/* Open the shared memory object */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	
	KillCheck(name);
	
	/* Configure the size of shared memory object */
	ftruncate(shm_fd, SIZE);
	
	KillCheck(name);
	
	/* Memory mapping the shared object */
	ptr = mmap(NULL, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	
	KillCheck(name);
	
	/* Write the 10 msgs to shared memory space */
	sprintf(ptr, "%s", msg1);
	LogFile_MsgSend(2, msg1);
	ptr += strlen(msg1);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg2);
	LogFile_MsgSend(2, msg2);
	ptr += strlen(msg2);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg3);
	LogFile_MsgSend(2, msg3);
	ptr += strlen(msg3);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg4);
	LogFile_MsgSend(2, msg4);
	ptr += strlen(msg4);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg5);
	LogFile_MsgSend(2, msg5);
	ptr += strlen(msg5);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg6);
	LogFile_MsgSend(2, msg6);
	ptr += strlen(msg6);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg7);
	LogFile_MsgSend(2, msg7);
	ptr += strlen(msg7);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg8);
	LogFile_MsgSend(2, msg8);
	ptr += strlen(msg8);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg9);
	LogFile_MsgSend(2, msg9);
	ptr += strlen(msg9);
	
	KillCheck(name);
	
	sprintf(ptr, "%s", msg10);
	LogFile_MsgSend(2, msg10);
	ptr += strlen(msg10);
	
	KillCheck(name);
	
	/* Wait for pthread1 to write to its shared memory */
	sleep(3);
	
	/* size in bytes of the shared memory object */
	const int SIZE_R = 4096; 
	
	/* Name of the shared memory object */
	const char* name_R = "/SharedMem_P1";
	
	/* Shared memory file discriptor */
	int shm_fd_R; 
	
	/* Pointer to the shared memory object */
	void *ptr_R; 
	
	/* Open the shared memory object */
	shm_fd_R = shm_open(name_R, O_RDONLY, 0666);
	
	KillCheck(name_R);
	
	/* Memory mapping the shared object */
	ptr_R = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, shm_fd_R, 0);
	
	KillCheck(name_R);
	
	/* Reading 10 msgs from shared memory space */
	LogFile_MsgReceived(2, (char *)ptr_R);
	
	KillCheck(name_R);
	
	/* Remove link to share object */
	shm_unlink(name_R);
	
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
	fprintf(MyFileP, "Starting Shared Memory IPC Demo...\n");
	fclose(MyFileP);
	
	pthread_create(&Thread1, NULL, &Thread1_SharedMem, NULL);
	pthread_create(&Thread2, NULL, &Thread2_SharedMem, NULL);
	
	/* Wait for pthreads to finish */
	pthread_join(Thread1, NULL);
	pthread_join(Thread2, NULL);
	
	printf("Done!\n");
	
	return 0;
}
