#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

	int
sys_fork(void)
{
	return fork();
}

	int
sys_exit(void)
{
	exit();
	return 0;  // not reached
}

	int
sys_wait(void)
{
	return wait();
}

	int
sys_kill(void)
{
	int pid;

	if(argint(0, &pid) < 0)
		return -1;
	return kill(pid);
}

	int
sys_getpid(void)
{
	return myproc()->pid;
}

	int
sys_sbrk(void)
{
	int addr;
	int n;

	if(argint(0, &n) < 0)
		return -1;
	addr = myproc()->sz;
	if(growproc(n) < 0)
		return -1;
	return addr;
}

	int
sys_sleep(void)
{
	int n;
	uint ticks0;

	if(argint(0, &n) < 0)
		return -1;
	acquire(&tickslock);
	ticks0 = ticks;
	while(ticks - ticks0 < n){
		if(myproc()->killed){
			release(&tickslock);
			return -1;
		}
		sleep(&ticks, &tickslock);
	}
	release(&tickslock);
	return 0;
}

// return how many clock tick interrupts have occurred
// since start.
	int
sys_uptime(void)
{
	uint xticks;

	acquire(&tickslock);
	xticks = ticks;
	release(&tickslock);
	return xticks;
}


/*************** Lab Quiz 3 ***************/
#define NLOCKS 20

struct semaphore_t
{
	/*
	   Semaphore related variables
	 */
	int sem_value;
	struct spinlock s;
};

int counters[NLOCKS];
struct semaphore_t semaphores[NLOCKS];

// Semaphore implementation from here on.
int 
sys_sem_init(void)
{  
	// .....
	// .....
	// .....
	// .....
	int index , value ;
	if(argint(0,&index) < 0 || argint(1,&value) < 0)
	{
		return -1;
	}

	if(index < 0 || index >= NLOCKS)
	{
		return -1;
	}

	semaphores[index].sem_value  = value ;
	semaphores[index].s.locked = 0;

	return 1;
}

int
sys_sem_down(void)
{
	// .....
	// .....
	int index;
	if(argint(0,&index) < 0)
	{
		return -1;
	}

	if(index < 0 || index >= NLOCKS)
	{
		return -1;
	}

	spinlock_acquire(&semaphores[index].s);

	while(semaphores[index].sem_value == 0)
	{
		cv_sleep((void *)index,&semaphores[index].s) ;
	}

	semaphores[index].sem_value--;

	cprintf("Process: %d, sem[%d]: %d \n", sys_getpid(), index, semaphores[index].sem_value);
	// .....
	// .....
	spinlock_release(&semaphores[index].s) ;
	return 1;
}

int
sys_sem_up(void)
{
	// .....
	// .....
	int index;
	if(argint(0,&index) < 0)
	{
		return -1;
	}

	if(index < 0 || index >= NLOCKS)
	{
		return -1;
	}

	spinlock_acquire(&semaphores[index].s);

	semaphores[index].sem_value++ ;
	wakeup((void *)index);

	cprintf("Process: %d, sem[%d]: %d \n", sys_getpid(), index, semaphores[index].sem_value);
	// .....
	// .....
	spinlock_release(&semaphores[index].s);

	return 1;
}

// Shared variable related system calls.
int 
sys_init_counters(void)
{
	for(int i=0; i<NLOCKS; i++)
		counters[i] = 0;

	return 1;
}

int 
sys_get_var(void)
{
	int index;

	if((argint(0, &index) < 0) || index < 0 || index >=NLOCKS )
		return -1; 

	return counters[index];
}


int 
sys_set_var(void)
{
	int index;
	int value;

	if((argint(0, &index) < 0) || (argint(1, &value) < 0) || index < 0 || index >= NLOCKS )
		return -1; 

	counters[index] = value;

	return 1;
}
/*************** Lab Quiz 3 ***************/
