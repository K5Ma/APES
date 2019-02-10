#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>


int main()
{  
	/* This will store the return value from my system call */
	long int Sys_return;
	printf("> Invoking 'my_arr_sort' system call...\n");

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
		printf("> SUCCESS: System call 'my_arr_sort' complete!\n");
		/* Printing sorted array in userspace */
		printf("> Printing sorted array got from kernelspace:\n");
		for (int32_t i = 0; i < 256; i++)
		{
			printf("> Sorted_NumArr[%d] = %d\n: ", i, Sorted_NumArr[i]);
		}
	}
	else
	{
		printf("> ERROR: System call 'my_arr_sort' did not execute as expected\nReturn code: %d\n", Sys_return);
		perror("> perror:\n");
	}         
	return 0;
}  
