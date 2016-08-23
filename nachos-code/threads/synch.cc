// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List<Thread*>;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append(currentThread);		// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    interrupt->SetLevel(oldLevel);		// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

//Parece que esto es codigo viejo
void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) {
	name = debugName;
	semLock = new Semaphore (name,1);
	tieneLock = NULL;
}
	
Lock::~Lock() {
	delete semLock;
}
	

bool Lock::isHeldByCurrentThread() {
		if (tieneLock == currentThread)
			return true;
		else
			return false;
}

void Lock::Acquire() {
	ASSERT (!isHeldByCurrentThread());
	semLock -> P();
	tieneLock = currentThread;
}
void Lock::Release() {
	ASSERT (isHeldByCurrentThread());
	tieneLock = NULL;
	semLock -> V();

}

Condition::Condition(const char* debugName, Lock* conditionLock) { 
		name = debugName;
		condLock = conditionLock;
		enWait = new List<Semaphore*>;
}
Condition::~Condition() { 
		delete enWait;
}
void Condition::Wait() { 
	ASSERT(condLock ->isHeldByCurrentThread()); 
	Semaphore* sem= new Semaphore (currentThread->getName(),0);
	enWait->Append(sem);
	condLock -> Release();
	sem -> P();
	condLock -> Acquire();
	delete sem;
}
void Condition::Signal() { 
	ASSERT(condLock ->isHeldByCurrentThread());
	if (enWait->IsEmpty())
		return;
	else 
		enWait->Remove()->V();
	
}
void Condition::Broadcast() { 
	ASSERT(condLock ->isHeldByCurrentThread());
	while (! enWait->IsEmpty())
		enWait->Remove()->V();	
}


