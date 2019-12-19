i

Function for obtaining the spinlock - void acquire(struct spinlock *lk);
Function for releasing the spinlock - void release(struct spinlock *lk);

ii

Check whether the cpu (the current cpu which runs the code) is holding the lock.

iii

writing acquire twice, one after the other before releasing the lock.
So, when we try to acquire the same lock for the second time without releasing it,
in the acquire function the holding(lk) evaluates to true and we get the panic(acquire)
code is invoked.

iv

v

If sti() is added after acquire , this might create a problem.
Because interrupts can occur and lead to a deadlock state (the first entity might not
release the lock and the second entity tries to acquire it.)

cli() before release wont be a problem as we are clearing interrupts.
