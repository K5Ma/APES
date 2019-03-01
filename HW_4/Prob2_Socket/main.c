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

/* Socket code was taken from: https://www.geeksforgeeks.org/socket-programming-cc/
 * and then heavily modified */

/* Defines */
#define PORT 8080 

/* Global Varibales */
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


/* 
 * This functoin checks if the user wanted to kill demo :(
 */
void KillCheck()
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
		fprintf(MyFileP, "#      Signal value: %d                #\n", SigNum);
		fprintf(MyFileP, "#      Recvieved at: %ld.%ld          #\n", time.tv_sec, time.tv_usec);
		fprintf(MyFileP, "#######################################\n");
		
		/* Close the file */
		fclose(MyFileP);
		
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




void * Thread1_Server(void * args)
{
	printf("**DEBUG** Thread 1 reached!\n");
	
	LogFile_Init();
	
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	
	int addrlen = sizeof(address);
	
	//Creating socket file descriptor
	if( (server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0 )
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	//Forcefully attaching socket to the port 8080
	if( setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) ) 
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );	
	
	//Forcefully attaching socket to the port 8080
	if( bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	if( listen(server_fd, 3) < 0 )
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	
	if( (new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0 )
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	
	
	/* Recieve and then send a message - total of 20 messages */
	MsgStruct MsgP1;		//Used to send messages
	MsgStruct MsgRecv;		//Used to store recieved messages
	
	//1 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//2 - S
	strcpy(MsgP1.Msg, "Hi Pthread2");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//3 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//4 - S
	strcpy(MsgP1.Msg, "I'm good, you?");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//5 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//6 - S
	strcpy(MsgP1.Msg, "What are we doing?");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//7 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//8 - S
	strcpy(MsgP1.Msg, "Huh... cool...");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//9 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//10 - S
	strcpy(MsgP1.Msg, "Logging as well? Cool.");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//11 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//12 - S
	strcpy(MsgP1.Msg, "I see.. so a total of 20 messages?");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//13 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//14 - S
	strcpy(MsgP1.Msg, "How much is left?");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//15 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//16 - S
	strcpy(MsgP1.Msg, "Oh damn..");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//17 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//18 - S
	strcpy(MsgP1.Msg, "Well, goodbye Pthread2");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	KillCheck();
	
	//19 - R
	valread = read( new_socket, &MsgRecv, sizeof(MsgStruct) );
	LogFile_MsgReceived(1, &MsgRecv);
	
	KillCheck();
	
	//20 - S
	strcpy(MsgP1.Msg, "I am alone... goodbye...");
	MsgP1.Len = strlen(MsgP1.Msg);
	MsgP1.LED_Cntrl = true;
	send(new_socket , &MsgP1, sizeof(MsgStruct), 0);
	LogFile_MsgSend(1, &MsgP1);
	
	printf("**DEBUG** Exiting Thread 1...\n");
	
	return NULL;
}


void * Thread2_Client(void * args)
{
	printf("**DEBUG** Thread 2 reached!\n");
	
	LogFile_Init();
	
	struct sockaddr_in address;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	
	char *hello = "Hello from client";
	char buffer[1024] = {0};
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("**DEBUG** Thread 2: Socket creation error \n");
		return -1;
	}
	
	memset(&serv_addr, '0', sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("**DEBUG** Thread 2: Invalid address / Address not supported \n");
		return -1;
	}
	
	if( connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 )
	{
		printf("**DEBUG** Thread 2: Connection Failed \n");
		return -1;
	}
	
	KillCheck();
	
	/* Send then recieve a message - total of 20 messages */
	MsgStruct MsgP2;		//Used to send messages
	MsgStruct MsgRecv;		//Used to store recieved messages
	
	KillCheck();
	
	//1 - S
	strcpy(MsgP2.Msg, "Hello Pthread1");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//2 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	KillCheck();
	
	//3 - S
	strcpy(MsgP2.Msg, "How are you?");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//4 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	KillCheck();
	
	//5 - S
	strcpy(MsgP2.Msg, "I'm good");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//6 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	KillCheck();
	
	//7 - S
	strcpy(MsgP2.Msg, "We are demoing IPC using Sockets");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//8 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	
	KillCheck();
	
	//9 - S
	strcpy(MsgP2.Msg, "Yeah.. and we are logging as well");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//10 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	KillCheck();
	
	//11 - S
	strcpy(MsgP2.Msg, "Each thread will send 10 messages");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//12 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	KillCheck();
	
	//13 - S
	strcpy(MsgP2.Msg, "Yes");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//14 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	KillCheck();
	
	//15 - S
	strcpy(MsgP2.Msg, "You have 3 messages left to send, I have 2");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//16 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	KillCheck();
	
	//17 - S
	strcpy(MsgP2.Msg, "Yeah...");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//18 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
	KillCheck();
	
	//19 - S
	strcpy(MsgP2.Msg, "Goodbye u_u");
	MsgP2.Len = strlen(MsgP2.Msg);
	MsgP2.LED_Cntrl = true;
	send(sock, &MsgP2, sizeof(MsgStruct), 0);
	LogFile_MsgSend(2, &MsgP2);
	
	KillCheck();
	
	//20 - R
	valread = read(sock, &MsgRecv, sizeof(MsgStruct));
	LogFile_MsgReceived(2, &MsgRecv);
	
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
	fprintf(MyFileP, "Starting Socket IPC Demo...\n");
	fclose(MyFileP);
	
	KillCheck();
	
	pthread_create(&Thread1, NULL, &Thread1_Server, NULL);
	sleep(1);														//This garantees that the Server will start first (pthread1)
	pthread_create(&Thread2, NULL, &Thread2_Client, NULL);
	
	/* Wait for pthreads to finish */
	pthread_join(Thread1, NULL);
	
	KillCheck();
	
	pthread_join(Thread2, NULL);
	
	KillCheck();
	
	printf("Done!\n");
	
	return 0;
}
