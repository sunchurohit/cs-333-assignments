#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

/***************** LAB QUIZ 4 *****************/
#define MAX_TASKS 100

enum taskstatus { FREE, PENDING, ALLOCATED, COMPLETED };
	// FREE    	 : The task object is free and can be used to store a task
	// PENDING 	 : Task has been issued by master and is pending for execution
	// ALLOCATED : One of the worker process has started executing the task,
	//			   but not finished the execution
	// COMPLETED : Worker process has completed execution of the task, but master
	//			   has not consumed task completion, and task state is not yet 
	//			   available for re-use


struct task_t
{
	enum taskstatus status;  //  0-Free(Unused),    1-Pending,    2-Allocated.    3-Completed
	int task_id;             //  Identifier of the task
	int arg1;                //  Arguments to the function				
	int arg2;                
	int result;              //  Result of task
	char* fptr;              //  Pointer to the function defining the task
};

struct taskmaster_t
{
	int cv1;               //  Conditional variables
	int cv2;               //  Conditional variables
	//	.......

	struct spinlock lock; 
	struct task_t tasks[MAX_TASKS]; //  Array of tasks
};

struct taskmaster_t taskmaster;
/***************** LAB QUIZ 4 *****************/

struct {
	struct spinlock lock;
	struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

	void
pinit(void)
{
	initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
	return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
	struct cpu*
mycpu(void)
{
	int apicid, i;

	if(readeflags()&FL_IF)
		panic("mycpu called with interrupts enabled\n");

	apicid = lapicid();
	// APIC IDs are not guaranteed to be contiguous. Maybe we should have
	// a reverse map, or reserve a register to store &cpus[i].
	for (i = 0; i < ncpu; ++i) {
		if (cpus[i].apicid == apicid)
			return &cpus[i];
	}
	panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
	struct cpu *c;
	struct proc *p;
	pushcli();
	c = mycpu();
	p = c->proc;
	popcli();
	return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
	static struct proc*
allocproc(void)
{
	struct proc *p;
	char *sp;

	acquire(&ptable.lock);

	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
		if(p->state == UNUSED)
			goto found;

	release(&ptable.lock);
	return 0;

found:
	p->state = EMBRYO;
	p->pid = nextpid++;

	release(&ptable.lock);

	// Allocate kernel stack.
	if((p->kstack = kalloc()) == 0){
		p->state = UNUSED;
		return 0;
	}

	if((p->oldtf = (struct trapframe*) kalloc()) == 0)
	{
		return 0;
	}

	sp = p->kstack + KSTACKSIZE;

	// Leave room for trap frame.
	sp -= sizeof *p->tf;
	p->tf = (struct trapframe*)sp;

	// Set up new context to start executing at forkret,
	// which returns to trapret.
	sp -= 4;
	*(uint*)sp = (uint)trapret;

	sp -= sizeof *p->context;
	p->context = (struct context*)sp;
	memset(p->context, 0, sizeof *p->context);
	p->context->eip = (uint)forkret;

	return p;
}

//PAGEBREAK: 32
// Set up first user process.
	void
userinit(void)
{
	struct proc *p;
	extern char _binary_initcode_start[], _binary_initcode_size[];

	p = allocproc();

	initproc = p;
	if((p->pgdir = setupkvm()) == 0)
		panic("userinit: out of memory?");
	inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
	p->sz = PGSIZE;
	memset(p->tf, 0, sizeof(*p->tf));
	p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
	p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
	p->tf->es = p->tf->ds;
	p->tf->ss = p->tf->ds;
	p->tf->eflags = FL_IF;
	p->tf->esp = PGSIZE;
	p->tf->eip = 0;  // beginning of initcode.S

	safestrcpy(p->name, "initcode", sizeof(p->name));
	p->cwd = namei("/");

	// this assignment to p->state lets other cores
	// run this process. the acquire forces the above
	// writes to be visible, and the lock is also needed
	// because the assignment might not be atomic.
	acquire(&ptable.lock);

	p->state = RUNNABLE;

	release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
	int
growproc(int n)
{
	uint sz;
	struct proc *curproc = myproc();

	sz = curproc->sz;
	if(n > 0){
		if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
			return -1;
	} else if(n < 0){
		if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
			return -1;
	}
	curproc->sz = sz;
	switchuvm(curproc);
	return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
	int
fork(void)
{
	int i, pid;
	struct proc *np;
	struct proc *curproc = myproc();

	// Allocate process.
	if((np = allocproc()) == 0){
		return -1;
	}

	// Copy process state from proc.
	if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
		kfree(np->kstack);
		np->kstack = 0;
		np->state = UNUSED;
		return -1;
	}
	np->sz = curproc->sz;
	np->parent = curproc;
	*np->tf = *curproc->tf;

	// Clear %eax so that fork returns 0 in the child.
	np->tf->eax = 0;

	for(i = 0; i < NOFILE; i++)
		if(curproc->ofile[i])
			np->ofile[i] = filedup(curproc->ofile[i]);
	np->cwd = idup(curproc->cwd);

	safestrcpy(np->name, curproc->name, sizeof(curproc->name));

	pid = np->pid;

	acquire(&ptable.lock);

	np->state = RUNNABLE;

	release(&ptable.lock);

	return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
	void
exit(void)
{
	struct proc *curproc = myproc();
	struct proc *p;
	int fd;

	if(curproc == initproc)
		panic("init exiting");

	// Close all open files.
	for(fd = 0; fd < NOFILE; fd++){
		if(curproc->ofile[fd]){
			fileclose(curproc->ofile[fd]);
			curproc->ofile[fd] = 0;
		}
	}

	begin_op();
	iput(curproc->cwd);
	end_op();
	curproc->cwd = 0;

	acquire(&ptable.lock);

	// Parent might be sleeping in wait().
	wakeup1(curproc->parent);

	// Pass abandoned children to init.
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->parent == curproc){
			p->parent = initproc;
			if(p->state == ZOMBIE)
				wakeup1(initproc);
		}
	}

	// Jump into the scheduler, never to return.
	curproc->state = ZOMBIE;
	sched();
	panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
	int
wait(void)
{
	struct proc *p;
	int havekids, pid;
	struct proc *curproc = myproc();

	acquire(&ptable.lock);
	for(;;){
		// Scan through table looking for exited children.
		havekids = 0;
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			if(p->parent != curproc)
				continue;
			havekids = 1;
			if(p->state == ZOMBIE){
				// Found one.
				pid = p->pid;
				kfree(p->kstack);
				p->kstack = 0;
				freevm(p->pgdir);
				p->pid = 0;
				p->parent = 0;
				p->name[0] = 0;
				p->killed = 0;
				p->state = UNUSED;
				release(&ptable.lock);
				return pid;
			}
		}

		// No point waiting if we don't have any children.
		if(!havekids || curproc->killed){
			release(&ptable.lock);
			return -1;
		}

		// Wait for children to exit.  (See wakeup1 call in proc_exit.)
		sleep(curproc, &ptable.lock);  //DOC: wait-sleep
	}
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
	void
scheduler(void)
{
	struct proc *p;
	struct cpu *c = mycpu();
	c->proc = 0;

	for(;;){
		// Enable interrupts on this processor.
		sti();

		// Loop over process table looking for process to run.
		acquire(&ptable.lock);
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			if(p->state != RUNNABLE)
				continue;

			// Switch to chosen process.  It is the process's job
			// to release ptable.lock and then reacquire it
			// before jumping back to us.
			c->proc = p;
			switchuvm(p);
			p->state = RUNNING;

			swtch(&(c->scheduler), p->context);
			switchkvm();

			// Process is done running for now.
			// It should have changed its p->state before coming back.
			c->proc = 0;
		}
		release(&ptable.lock);

	}
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
	void
sched(void)
{
	int intena;
	struct proc *p = myproc();

	if(!holding(&ptable.lock))
		panic("sched ptable.lock");
	if(mycpu()->ncli != 1)
		panic("sched locks");
	if(p->state == RUNNING)
		panic("sched running");
	if(readeflags()&FL_IF)
		panic("sched interruptible");
	intena = mycpu()->intena;
	swtch(&p->context, mycpu()->scheduler);
	mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
	void
yield(void)
{
	acquire(&ptable.lock);  //DOC: yieldlock
	myproc()->state = RUNNABLE;
	sched();
	release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
	void
forkret(void)
{
	static int first = 1;
	// Still holding ptable.lock from scheduler.
	release(&ptable.lock);

	if (first) {
		// Some initialization functions must be run in the context
		// of a regular process (e.g., they call sleep), and thus cannot
		// be run from main().
		first = 0;
		iinit(ROOTDEV);
		initlog(ROOTDEV);
	}

	// Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
	void
sleep(void *chan, struct spinlock *lk)
{
	struct proc *p = myproc();

	if(p == 0)
		panic("sleep");

	if(lk == 0)
		panic("sleep without lk");

	// Must acquire ptable.lock in order to
	// change p->state and then call sched.
	// Once we hold ptable.lock, we can be
	// guaranteed that we won't miss any wakeup
	// (wakeup runs with ptable.lock locked),
	// so it's okay to release lk.
	if(lk != &ptable.lock){  //DOC: sleeplock0
		acquire(&ptable.lock);  //DOC: sleeplock1
		release(lk);
	}
	// Go to sleep.
	p->chan = chan;
	p->state = SLEEPING;

	sched();

	// Tidy up.
	p->chan = 0;

	// Reacquire original lock.
	if(lk != &ptable.lock){  //DOC: sleeplock2
		release(&ptable.lock);
		acquire(lk);
	}
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
	static void
wakeup1(void *chan)
{
	struct proc *p;

	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
		if(p->state == SLEEPING && p->chan == chan)
			p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
	void
wakeup(void *chan)
{
	acquire(&ptable.lock);
	wakeup1(chan);
	release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
	int
kill(int pid)
{
	struct proc *p;

	acquire(&ptable.lock);
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->pid == pid){
			p->killed = 1;
			// Wake process from sleep if necessary.
			if(p->state == SLEEPING)
				p->state = RUNNABLE;
			release(&ptable.lock);
			return 0;
		}
	}
	release(&ptable.lock);
	return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
	void
procdump(void)
{
	static char *states[] = {
		[UNUSED]    "unused",
		[EMBRYO]    "embryo",
		[SLEEPING]  "sleep ",
		[RUNNABLE]  "runble",
		[RUNNING]   "run   ",
		[ZOMBIE]    "zombie"
	};
	int i;
	struct proc *p;
	char *state;
	uint pc[10];

	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->state == UNUSED)
			continue;
		if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
			state = states[p->state];
		else
			state = "???";
		cprintf("%d %s %s", p->pid, state, p->name);
		if(p->state == SLEEPING){
			getcallerpcs((uint*)p->context->ebp+2, pc);
			for(i=0; i<10 && pc[i] != 0; i++)
				cprintf(" %p", pc[i]);
		}
		cprintf("\n");
	}
}

int init_taskmaster(void)
{
	taskmaster.cv1 = 0;
	taskmaster.cv2 = 0;

	// taskmaster.lock.locked = 0;

	initlock(&taskmaster.lock,"taskmaster lock!");

	for(int i=0;i<MAX_TASKS;i++)
	{
		taskmaster.tasks[i].status = FREE ;
	}

	return 1;
}

int do_task(char* fptr, int task_id, int arg1, int arg2)
{
	int already_used = 0;
	int found = 0;

	for(int i=0;i<MAX_TASKS;i++)
	{
		if(taskmaster.tasks[i].status == FREE)
		{
			found = 1;
			break;
		}
	}

	for(int i=0;i<MAX_TASKS;i++)
	{
		if(taskmaster.tasks[i].status != FREE && taskmaster.tasks[i].task_id == task_id)
		{
			already_used = 1; break;
		}
	}

	if(already_used && !found)
	{
		cprintf("Error in do_task!!\n");
		return -1;
	}

	for(int i=0;i<MAX_TASKS;i++)
	{
		if(taskmaster.tasks[i].status == FREE)
		{
			taskmaster.tasks[i].status = PENDING ;
			taskmaster.tasks[i].task_id = task_id ;
			//cprintf("yuy%d\n",taskmaster.tasks[i].task_id) ;
			taskmaster.tasks[i].arg1 = arg1;
			taskmaster.tasks[i].arg2 = arg2;
			taskmaster.tasks[i].fptr = fptr;
			// dont do anything with the result!!
			break;
		}
	}
	//need to wakeup worker processes.
	// taskmaster.cv1 = 1;

	wakeup((void *) taskmaster.cv1);

	return 1;
}

int wait_for_task_to_complete(int task_id, int* result)
{
	int exists = 0;
	for(int i=0;i< MAX_TASKS;i++)
	{
		if(taskmaster.tasks[i].task_id == task_id)
		{
			exists = 1; break ;
		}
	}

	if(!exists)
	{
		return -1;
	}

	while(1)
	{
		for(int i=0;i<MAX_TASKS;i++)
		{
		if(taskmaster.tasks[i].task_id == task_id)
			{
			if(taskmaster.tasks[i].status != COMPLETED)
				{
					acquire(&taskmaster.lock);
					sleep((void *) taskmaster.cv2, &taskmaster.lock) ;
					release(&taskmaster.lock);
				}
			else
				{
					*result = taskmaster.tasks[i].result ;
					// wakeup((void *) taskmaster.cv1);
					return *result ;

				}
			}
		}	
	}

	

	cprintf("Some error in wait_for_task_to_complete!!\n");
	return -1;
}

int wait_for_task(void)
{
	struct proc *p = myproc();

	while(1)
	{
		for(int i=0;i<MAX_TASKS;i++)
		{	
		if(taskmaster.tasks[i].status == PENDING)
			{
				taskmaster.tasks[i].status = ALLOCATED ;
				*(p->oldtf) = *(p->tf) ;
				p->tf->esp -= 4;
				*((int *) p->tf->esp) = taskmaster.tasks[i].arg2 ;
				p->tf->esp -= 4;
				*((int *) p->tf->esp) = taskmaster.tasks[i].arg1 ;
				p->tf->esp -= 4;
				*((int *) p->tf->esp) = taskmaster.tasks[i].task_id ;
				p->tf->esp -= 4; // for the return address.
				p->tf->eip = (uint) taskmaster.tasks[i].fptr ; // changing eip, runs the required function.
				return 1;
			}
		}
		acquire(&taskmaster.lock);
		sleep((void *) taskmaster.cv1 , &taskmaster.lock) ;
		release(&taskmaster.lock);
	}
}

int task_ret(int task_id, int result)
{
	// cprintf("hey!\n");
	struct proc *p = myproc();
	for(int i=0;i<MAX_TASKS;i++)
	{
		// cprintf("%d\n", taskmaster.tasks[i].task_id);
		if(taskmaster.tasks[i].task_id == task_id)
		{
			taskmaster.tasks[i].status = COMPLETED ;
			taskmaster.tasks[i].result = result ;
			wakeup((void *) taskmaster.cv2) ;
			*(p->tf) = *(p->oldtf) ;
			return task_id;
		}
	}
	cprintf("Task with task_id = %d not found\n", task_id);
	return -1;
}
