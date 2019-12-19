#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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

/***************** LAB QUIZ 4 *****************/
int sys_init_taskmaster(void)
{
  return init_taskmaster() ;
  // return 1;
}

int sys_do_task(void)
{
	char* fptr ; int task_id ; int arg1; int arg2 ;
	if(argptr(0,&fptr,4) < 0 || argint(1,&task_id) < 0 || argint(2,&arg1) < 0 || argint(3,&arg2) < 0)
	{
		return -1;
	}
	return do_task(fptr,task_id,arg1,arg2) ;
  //return 1;
}

int sys_wait_for_task_to_complete(void)
{
	int task_id ; int* result ; char* temp;
	if(argint(0,&task_id) < 0 || argptr(1,&temp,4) < 0)
	{
		return -1;
	}
	result = (int*) temp;
	return wait_for_task_to_complete(task_id,result);
  	// return 1;
}

int sys_wait_for_task(void)
{
	return wait_for_task();
	 // return 1;
}


int sys_task_ret(void)
{
	int task_id;  int result ;
	if(argint(0,&task_id) < 0 || argint(1,&result) < 0)
	{
		return -1;
	}

	return task_ret(task_id,result) ;

  	// return 1;
}
/***************** LAB QUIZ 4 *****************/
