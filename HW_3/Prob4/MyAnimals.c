#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/timer.h>



/* Default inputs */
static char *InStr = "";
static int CountGreater = -1;

module_param(CountGreater, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(CountGreater, "Number greater than to display");
module_param(InStr, charp, 0000);
MODULE_PARM_DESC(InStr, "Animal you want to display");


/* My animal structure */
struct Animals
{
	char AnimalName[30];		//Name
	unsigned int Count;			//Count
	struct list_head list;		//Kernel's list structure
};
struct Animals AnimalList_Set1;
struct Animals AnimalList_Set2;


/* This function will alphabetically sort our input array
 * found on: 
 * https://beginnersbook.com/2015/02/c-program-to-sort-set-of-strings-in-alphabetical-order/
 * modifed heavily */
void SortArray_Alphabetically(char arr[][30], uint8_t ArraySize)
{
	int i,j;
	char temp[30];
	
	for(i = 0; i < ArraySize; i++)
	{
		for(j = i+1; j < ArraySize; j++)
		{
			if( strcmp(arr[i], arr[j]) > 0)
			{
				strcpy(temp, arr[i]);
				strcpy(arr[i], arr[j]);
				strcpy(arr[j], temp);
			}
		}
	}
}


/* Global Varibles */
uint32_t MallocedBytes_Set1 = 0;
uint32_t MallocedBytes_Set2 = 0;


static int __init InitMy_AnimalsModule(void)
{
	struct timespec start;
	struct timespec end;

	/* Get start time and save it */
	getnstimeofday(&start);
	
	printk(KERN_ALERT "Installing My_AnimalsModule...\n\n");
	printk(KERN_ALERT "Populating array...\n");
	/* Create the array of 50 random animals */
	char Animal_Arr[][30] = {
							 "Zebra", "Zebra", "Dog", "Dog", "Dog", "Dog", "Frog", "Frog", "Fish", "Zebra",
							 "Snail", "Turtle", "Snake", "Turtle", "Horse", "Horse", "Snake", "Rabbit", "Fish", "Turtle",
							 "Zebra", "Horse", "Eagle", "Cat", "Dog", "Eagle", "Rabbit", "Rabbit", "Toad", "Turtle",
							 "Parrot", "Zebra", "Eagle", "Dog", "Cat", "Cat", "Koala", "Frog", "Fish", "Goat",
							 "Goat", "Goat", "Koala", "Dog", "Koala", "Dog", "Parrot", "Rabbit", "Fish", "Toad"
							};

	/* Get size of array */
	uint8_t Arr_Size = sizeof(Animal_Arr)/sizeof(Animal_Arr[0]);
	printk(KERN_ALERT "Size of inital array is: %u\n\n", Arr_Size);
	
	/* Sort array alphabetically */
	SortArray_Alphabetically(Animal_Arr, Arr_Size);
	
	unsigned int i;								//Used for looping
	int Animal_Found = 0;						//Flag to indcate if animal is found
	struct Animals *NewAnimal, *TempAnimal;		//Temp varibales

	/* Init the head of the list */
	INIT_LIST_HEAD(&AnimalList_Set1.list);
	/* Init the head of the list */
	INIT_LIST_HEAD(&AnimalList_Set2.list);

	/* Add animals from static array to AnimalList_Set1 */
	for(i=0; i < Arr_Size; ++i)
	{
		/* Check if animal exsists by looping */
		list_for_each_entry(TempAnimal, &AnimalList_Set1.list, list)
		{	
			if( strcmp(TempAnimal->AnimalName, Animal_Arr[i]) == 0 )
			{
				TempAnimal->Count++;
				printk(KERN_INFO "> Animal: '%s' exists in LL structure - incrementing count: %d\n", TempAnimal->AnimalName, TempAnimal->Count);
				Animal_Found = 1; 
				break;
			}
		}
		
		/* If Animal is not found, add it: */
		if(!Animal_Found)
		{
			NewAnimal = kmalloc(sizeof(*NewAnimal), GFP_KERNEL);
			
			MallocedBytes_Set1 += sizeof(*NewAnimal);
			
			strcpy(NewAnimal->AnimalName, Animal_Arr[i]);

			NewAnimal->Count = 1;

			INIT_LIST_HEAD(&NewAnimal->list);

			/* Add the new node to AnimalList_Set1 */
			list_add_tail(&(NewAnimal->list), &(AnimalList_Set1.list));
		}
		Animal_Found = 0;		//Reset flag
	}
	
	/* Printing Set1 with all the unique elements associated with count with duplicates removed: */
	printk(KERN_ALERT "Pritning Set1:\n");
	uint32_t NumNodes_1 = 0; 
	list_for_each_entry(TempAnimal, &AnimalList_Set1.list, list)
	{
		//Accesing each member of TempAnimal
		printk(KERN_INFO "> Animal name: %s - count: %d\n", TempAnimal->AnimalName, TempAnimal->Count);
		NumNodes_1++;
	}
	printk(KERN_ALERT "Number of nodes in Set1: %u\n", NumNodes_1);
	
	/* Three cases:
	 * - First: User has entered two filters 
	 * - Second: User has enetred only 1 filter 
	 * - Third: User entered no filters
	 */
	if( ((strcmp(InStr, "") != 0)) && (CountGreater > -1) )
	{
		printk(KERN_ALERT "* Two filters passed => Animal: '%s' and Count greater than: '%d' *", InStr, CountGreater);
		/* Copy from Set1 that ONLY match the filters */
		list_for_each_entry(TempAnimal, &AnimalList_Set1.list, list)
		{
			if( ( strcmp(InStr, TempAnimal->AnimalName) == 0 ) && (TempAnimal->Count > CountGreater) )
			{
				NewAnimal = kmalloc(sizeof(*NewAnimal), GFP_KERNEL);
				
				MallocedBytes_Set2 += sizeof(*NewAnimal);
				
				/* Copy info */
				strcpy(NewAnimal->AnimalName , TempAnimal->AnimalName);
				NewAnimal->Count = TempAnimal->Count;

				INIT_LIST_HEAD(&NewAnimal->list);

				/* Add the new node to AnimalList_Set2 */
				list_add_tail(&(NewAnimal->list), &(AnimalList_Set2.list));
			}
		}
	}
	else if( (strcmp(InStr, "") != 0)  && (CountGreater == -1) )
	{
		printk(KERN_ALERT "* One filter passed => Animal: '%s' *", InStr);
		/* Copy from Set1 that ONLY match the filter */
		list_for_each_entry(TempAnimal, &AnimalList_Set1.list, list)
		{
			if( strcmp(InStr, TempAnimal->AnimalName) == 0 )
			{
				NewAnimal = kmalloc(sizeof(*NewAnimal), GFP_KERNEL);
				
				MallocedBytes_Set2 += sizeof(*NewAnimal);
				
				/* Copy info */
				strcpy(NewAnimal->AnimalName , TempAnimal->AnimalName);
				NewAnimal->Count = TempAnimal->Count;

				INIT_LIST_HEAD(&NewAnimal->list);

				/* Add the new node to AnimalList_Set2 */
				list_add_tail(&(NewAnimal->list), &(AnimalList_Set2.list));
			}
		}
	}
	else if ( (strcmp(InStr, "") == 0 ) && (CountGreater > -1) )
	{
		printk(KERN_ALERT "* One filter passed => Count greater than: '%d' *", CountGreater);
		/* Copy from Set1 that ONLY match the filter */
		list_for_each_entry(TempAnimal, &AnimalList_Set1.list, list)
		{
			if(TempAnimal->Count > CountGreater)
			{
				NewAnimal = kmalloc(sizeof(*NewAnimal), GFP_KERNEL);
				
				MallocedBytes_Set2 += sizeof(*NewAnimal);
				
				/* Copy info */
				strcpy(NewAnimal->AnimalName , TempAnimal->AnimalName);
				NewAnimal->Count = TempAnimal->Count;

				INIT_LIST_HEAD(&NewAnimal->list);

				/* Add the new node to AnimalList_Set2 */
				list_add_tail(&(NewAnimal->list), &(AnimalList_Set2.list));
			}
		}
	}
	else
	{
		printk(KERN_ALERT "* No parameters passed *");
		/* Copy Set1 to Set2 as they are basically the same... */
		list_for_each_entry(TempAnimal, &AnimalList_Set1.list, list)
		{
			NewAnimal = kmalloc(sizeof(*NewAnimal), GFP_KERNEL);
			
			MallocedBytes_Set2 += sizeof(*NewAnimal);
			
			/* Copy info */
			strcpy(NewAnimal->AnimalName , TempAnimal->AnimalName);
			NewAnimal->Count = TempAnimal->Count;

			INIT_LIST_HEAD(&NewAnimal->list);

			/* Add the new node to AnimalList_Set2 */
			list_add_tail(&(NewAnimal->list), &(AnimalList_Set2.list));
		}
	}
	
	/* Printing Set2: */
	printk(KERN_ALERT "Pritning Set2:\n");
	uint32_t NumNodes_2 = 0; 
	list_for_each_entry(TempAnimal, &AnimalList_Set2.list, list)
	{
		//Accesing each member of TempAnimal
		printk(KERN_INFO "> Animal name: %s - count: %d\n", TempAnimal->AnimalName, TempAnimal->Count);
		NumNodes_2++;
	}
	printk(KERN_ALERT "Number of nodes in Set2: %u\n", NumNodes_2);

	/* Get end time and save it */
	getnstimeofday (&end);
	
	/* Get total time elapsed */
	struct timespec diff;
	diff = timespec_sub(end, start); 
	
	/* Print */
	printk(KERN_ALERT "\nTime elapsed in ms: %lu\n", (diff.tv_nsec / 1000000) );
	
	printk(KERN_INFO "Amount of bytes allocated for Set1: %d\n", MallocedBytes_Set1);
	printk(KERN_INFO "Amount of bytes allocated for Set2: %d\n", MallocedBytes_Set2);
	return 0; 
}

static void __exit ExitMy_AnimalsModule(void)
{
	struct timespec start;
	struct timespec end;

	/* Get start time and save it */
	getnstimeofday(&start);
	
	printk(KERN_ALERT "Uninstalling My_AnimalsModule...\n");
	
	/* Showing amount of allocated memory in bytes and tehn freeing it */
	struct Animals *TempAnimal;
	/* Free Set1 */
	printk(KERN_INFO "Freeing the %d bytes allocated for Set1...", MallocedBytes_Set1);
	list_for_each_entry(TempAnimal, &AnimalList_Set1.list, list)
	{	
		kfree(TempAnimal);
	}
	/* Free Set2 */
	printk(KERN_INFO "Freeing the %d bytes allocated for Set2...", MallocedBytes_Set2);
	list_for_each_entry(TempAnimal, &AnimalList_Set2.list, list)
	{	
		kfree(TempAnimal);
	}

	/* Get end time and save it */
	getnstimeofday (&end);
	
	/* Get total time elapsed */
	struct timespec diff;
	diff = timespec_sub(end, start); 
	
	/* Print */
	printk(KERN_ALERT "\nTime elapsed in ms: %lu\n", (diff.tv_nsec / 1000000) );

}

module_init(InitMy_AnimalsModule);
module_exit(ExitMy_AnimalsModule);
MODULE_DESCRIPTION("A kernel function that for whatever reason sorts animals in kernel space");
MODULE_AUTHOR("Khalid AlAwadhi");
MODULE_LICENSE("GPL");
