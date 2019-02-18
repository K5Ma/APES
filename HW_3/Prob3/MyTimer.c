#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/timer.h>

/* This code was based on an example found online:
   https://www.ibm.com/developerworks/library/l-timers-list/index.html
 */

static char *InputString = "Khalid";
static int Period_mS = 500;
static int TimesCalled = 0;

module_param(Period_mS, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(Period_mS, "The period");
module_param(InputString, charp, 0000);
MODULE_PARM_DESC(InputString, "A character string");


static struct timer_list MyTimer;


/* This function is called whenever the timer is done */
void My_Timer_Callback( unsigned long data )
{
	printk(KERN_ALERT "My_Timer_Callback reached!\n");
	
	TimesCalled++;
	
	printk(KERN_ALERT "Number of times called: %d\n", TimesCalled);
	printk(KERN_ALERT "Name: %s\n", InputString);
	
	int RetVal = mod_timer( &MyTimer, jiffies + msecs_to_jiffies(Period_mS) );
	if (RetVal)
	{
		printk(KERN_ALERT "ERROR in mod_timer!\n");
	}
}


static int __init InitMy_TimerModule(void)
{
	printk(KERN_ALERT "Installing My_TimerModule...\n");

	int RetVal;		//Stores the return value
	
	/* Setup timer */
	setup_timer( &MyTimer, My_Timer_Callback, 0);
	printk(KERN_ALERT "Starting timer to fire in %dms\n", Period_mS);
	
	RetVal = mod_timer( &MyTimer, jiffies + msecs_to_jiffies(Period_mS) );
	if (RetVal)
	{
		printk(KERN_ALERT "ERROR in mod_timer!\n");
		return -1;
	}
	
	return 0; 
}


static void __exit ExitMy_TimerModule(void)
{
	printk(KERN_ALERT "Uninstalling My_TimerModule...\n");
	del_timer( &MyTimer );
}

module_init(InitMy_TimerModule);
module_exit(ExitMy_TimerModule);
MODULE_DESCRIPTION("A kernel timer to periodically wake up every 500msec by default. Each time the timer wakes up it calls a function that prints info to the kernel log buffers");
MODULE_AUTHOR("Khalid AlAwadhi");
MODULE_LICENSE("GPL");
