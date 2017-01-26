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



void segundo(void* name){
    // Reinterpret arg "name" as a string
    char* ss = (char*)name;
    for(int i=1; i<=6; i++) {
        printf("escribe %s \n",ss);
        currentThread->Yield();
    }
    
}

void primer(void* name){
    // Reinterpret arg "name" as a string
    char* ss = (char*)name;
    currentThread->Yield();
    for(int i=1; i<=6; i++) {
        printf("escribe %s \n",ss);
        currentThread->Yield();
    }
}

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *test1 = new Thread ("uno", true);
    Thread *test2 = new Thread ("dos", true);
    test1 -> Fork (primer, (void*)"prim");
    test1 -> Join();
    test2 -> Fork (segundo, (void*)"seg");
    
}



