// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Parts from Copyright (c) 2007-2009 Universidad de Las Palmas de Gran Canaria
//

#include "copyright.h"
#include "system.h"
#include "synch.h"


/* 
//######### TESTEO LOCK ######### 

Lock *testLock;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------

void
SimpleThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread %s looped %d times\n", threadName, num);
	//interrupt->SetLevel(oldLevel);
       currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);
}

void
Test1Thread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    testLock -> Acquire();
    for (int num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread %s looped %d times\n", threadName, num);
	//interrupt->SetLevel(oldLevel);
       currentThread->Yield();
       }
    testLock -> Release();
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);
}



//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------


void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");
    testLock = new Lock("testLock");

    for ( int k=1; k<=5; k++) {
      char* threadname = new char[100];
	  sprintf(threadname, "Hilo %d", k);
      Thread* newThread = new Thread (threadname);
      //newThread->Fork (SimpleThread, (void*)threadname);
      newThread->Fork (Test1Thread, (void*)threadname);
    }
    
    //SimpleThread( (void*)"Hilo 0");
    Test1Thread( (void*)"Hilo 0");
}

*/

 
// ###### TESTEO CONDITION ########

/*
Lock *lockCond;
Condition *testCond; 


void
SimpleThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread %s looped %d times\n", threadName, num);
	//interrupt->SetLevel(oldLevel);
       currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);
}

void
Test1Th(void* name)
{	
	char* threadName = (char*)name;
    for (int num = 0; num < 10; num++) {

	printf("*** thread %s looped %d times\n", threadName, num);
       currentThread->Yield();
       }
    printf(">>> Thread %s has finished\n", threadName);
    lockCond -> Acquire();
    testCond->Signal();
    lockCond->Release();
}

void
Test2Th(void* name)
{	
	char* threadName = (char*)name;  
    lockCond -> Acquire();
    testCond->Wait();
    lockCond->Release();
    for (int num = 0; num < 10; num++) {

	printf("*** thread %s looped %d times\n", threadName, num);
       currentThread->Yield();
       }
    printf(">>> Thread %s has finished\n", threadName);
	lockCond -> Acquire();
    testCond->Signal();
    lockCond->Release();
}

void
Test3Th(void* name)
{
	lockCond -> Acquire();
	testCond->Wait();
    lockCond->Release();
    
    char* threadName = (char*)name;
    for (int num = 0; num < 10; num++) {

	printf("*** thread %s looped %d times\n", threadName, num);
       currentThread->Yield();
       }
    printf(">>> Thread %s has finished\n", threadName);
	lockCond -> Acquire();
    testCond->Signal();
    lockCond->Release();
}


void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");
    lockCond = new Lock("conditionlock");
	testCond = new Condition ("condition", lockCond);

	Thread *thread1 = new Thread ("hilo1");
	Thread *thread2 = new Thread ("hilo2");
	Thread *thread3 = new Thread ("hilo3");
	
	thread1->Fork (Test1Th, (void*)"hilo1");
	thread2->Fork (Test2Th, (void*)"hilo2");
	thread3->Fork (Test3Th, (void*)"hilo3");
}

*/

// ###### TESTEO PUERTOS ########

Port *testPort = new Port ("testPort");

void SendThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;

    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 1; num <= 3; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread S %s envio %d.\n", threadName, num);
        testPort -> Send (num);
	//interrupt->SetLevel(oldLevel);
        currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);

}

void SendThread2(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;

    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 6; num <= 8; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread S %s envio %d.\n", threadName, num);
        testPort -> Send (num);
	//interrupt->SetLevel(oldLevel);
        currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);

}

void ReceiveThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    int n;
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 1; num <= 6; num++) {
        testPort -> Receive (&n);
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread R %s recibio %d\n", threadName, n);
	//interrupt->SetLevel(oldLevel);
        currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);

}
void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *test1 = new Thread ("envia");
    Thread *test1b = new Thread ("envia2");
    Thread *test2 = new Thread ("recibe");
    test1 -> Fork (SendThread, (void*)"envia");
    test1b -> Fork (SendThread2, (void*)"envia2");
    test2 -> Fork (ReceiveThread, (void*)"recibe");
    
}
