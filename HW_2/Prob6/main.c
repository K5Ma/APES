#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	/* Create a file */
	FILE *MyFileP;

	/*  Open the file (in append mode) */
	MyFileP = fopen("/usr/bin/Prob6.txt", "a+");
	
	/* Write the string to the file */
	fprintf(MyFileP, "\n\n\n> RUNNING PROGRAM 'Prob6'...\n");


	//Current Process ID
	long int PID;
	PID = getpid();
	fprintf(MyFileP,"> Current Process ID: %ld\n", PID);
	
	//Current User ID
	long int UID;
	UID = getuid();
	fprintf(MyFileP,"> Current User ID: %ld\n", UID);
		
	//Current date and time
	struct tm *tm;
	time_t cur_time;
	char Date_string[100];
	char Time_String[100];

	cur_time = time(NULL);
	tm = localtime(&cur_time);

	strftime(Date_string, sizeof(Date_string), "%d %m %Y", tm);
	strftime(Time_String, sizeof(Time_String), "%H %M %S", tm);
	
	fprintf(MyFileP, "Date: %s\n", Date_string);
	fprintf(MyFileP, "Time: %s\n", Time_String);


	//Output of your system call
	/* This will store the return value from my system call */
	long int Sys_return;
	fprintf(MyFileP,"> Invoking 'my_arr_sort' system call...\n");

	/* Size of array */
	int32_t SizeArr = 256; 
	
	/* Creating array of SizeArr entries */
	int32_t NumArr[SizeArr];
	
	/* Filling array with random values */
	for (int32_t i = 0; i < 256; i++)
	{
		NumArr[i] = ( rand() % 1000 );
	}
	
	/* Sorted array */
	int32_t Sorted_NumArr[SizeArr];

	/* Calling my system call */
	Sys_return = syscall(398, &NumArr, SizeArr, &Sorted_NumArr); 

	if(Sys_return == 0)
	{
		fprintf(MyFileP, "> SUCCESS: System call 'my_arr_sort' complete!\n");
		/* Printing sorted array in userspace */
		fprintf(MyFileP, "> Printing sorted array got from kernelspace:\n");
		for (int32_t i = 0; i < 256; i++)
		{
			fprintf(MyFileP, "> Sorted_NumArr[%d] = %d\n: ", i, Sorted_NumArr[i]);
		}
	}
	else
	{
		fprintf(MyFileP,"> ERROR: System call 'my_arr_sort' did not execute as expected\nReturn code: %d\n", Sys_return);
	}
	
	/* Close the file */
	fclose(MyFileP);
       
	return 0;
}  

