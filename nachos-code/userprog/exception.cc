// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#include "readwritemem.h"
#include <string.h>




//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------



void incrementarPC(){
    int pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);
    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);
    machine->WriteRegister(NextPCReg, pc + 4);
    
}

struct Arg_WriteArgs{
        int c;
        char **v;
};

void startProc(void *arg) {
  currentThread->space->InitRegisters();
  currentThread->space->RestoreState();
  
  struct Arg_WriteArgs *a = (Arg_WriteArgs*) arg;
  currentThread->space->WriteArgs(a->c,a->v);
  
  delete a;
        
  machine->Run();
}



void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    
    DEBUG('e',"Excepcion %d wich %d \n",type,which);

    if (which == SyscallException) {
        switch(type){
            case SC_Halt:{
                DEBUG('a', "Shutdown, initiated by user program.\n");
   	            interrupt->Halt();
   	            break;
                }
            case SC_Create:{
                DEBUG('e',"Inicia SC_Create \n");
                int filenameAddr = machine->ReadRegister(4);
                char* filename = new char[128];
                ReadStringFromUser(filenameAddr, filename);
                fileSystem->Create(filename, 0);
                delete filename;
                incrementarPC();
                break;
                }
            case SC_Read:{
                int buffAddr = machine -> ReadRegister(4);
                int size = machine -> ReadRegister(5);
                OpenFileId fd = machine -> ReadRegister(6);
                
                char* buffer = new char[size];
                int i;
                
                if (fd == ConsoleOutput){
                    DEBUG('e',"Error: llamo a leer con ConsoleOutput \n");
                    machine -> WriteRegister(2,-1); // Error porque llamó a leer
                    incrementarPC();
                    break;
                } else if(fd == ConsoleInput){
                        for(i=0;i<size;i++){
                         buffer[i]= synchConsole->GetChar();   
                        }
                    } else{
                        OpenFile* of = currentThread->GetFile(fd);
                        if(of == NULL){
                            DEBUG('e',"Error: open file NULL \n");
                            machine -> WriteRegister(2,-1);
                            incrementarPC();
                            break;  
                        }
                        i = of -> Read(buffer, size);
                    }
                
                machine -> WriteRegister(2,i);
                WriteBufferToUser(buffer,buffAddr,i);
                delete[] buffer;
                incrementarPC();
                break;
            }
            case SC_Write:{
                int buffAddr = machine -> ReadRegister(4);
                int size = machine -> ReadRegister(5);
                OpenFileId fd = machine -> ReadRegister(6);
                char* buffer = new char[size];
                
                DEBUG('e',"entro a SC_Write con FD =  %d \n",fd);
                
                if (fd == ConsoleInput){
                    DEBUG('e',"Error: llamo a escribir con ConsoleInput \n");
                    machine -> WriteRegister(2,-1); // Error porque llamó a escribir
                    incrementarPC();
                    break;
                }
                
                ReadBufferFromUser(buffAddr,buffer,size);
                
                if (fd == ConsoleOutput){
                    DEBUG('e',"en SC_Write entro a consoleOutput \n");
                    for (int i=0; i<size;i++)
                        synchConsole -> PutChar(buffer[i]);
                } else{
                    OpenFile* of = currentThread->GetFile(fd);
                    if(of == NULL){
                        DEBUG('e',"Error: open file NULL \n");
                        machine -> WriteRegister(2,-1); // Error porque llamó a escribir
                        incrementarPC();
                        break;  
                    }
                    of->Write(buffer, size);
                }
                
                incrementarPC();
                delete[] buffer;
                break;
                
            }
            case SC_Open:{
                int fileAddr = machine->ReadRegister(4);
                char* name = new char[128];
                ReadStringFromUser(fileAddr, name);
                
                OpenFile* of = fileSystem->Open(name);
                
                delete[] name;
                
                if (of == NULL) {
                    DEBUG('e',"Error: open file NULL \n");
                    machine -> WriteRegister(2,-1);
                    incrementarPC();
                    break;
                } else {
                    OpenFileId fd = currentThread->AddFile(of);
                    machine->WriteRegister(2, fd);
                    incrementarPC();
                    break;
                }
                
            }
            case SC_Close:{
                OpenFileId fd = machine->ReadRegister(4);
                currentThread->RemoveFile(fd);
                incrementarPC();
                break;
                
            }
            case SC_Exit:{
                DEBUG('e',"Entro a SC_Exit \n");
                int ex = machine->ReadRegister(4);
                currentThread->setExStatus(ex);
                currentThread->Finish();
               // incrementarPC();
                break;
            }
            case SC_Join:{
                DEBUG('e',"Entra a SC_Join \n");
                int id = machine->ReadRegister(4);
                Thread* thread = processTable->GetProcess(id);
                if(thread == NULL){
                    DEBUG('e',"Error: thread NULL \n");
                    machine -> WriteRegister(2,-1);
                    incrementarPC();
                    break;
                } else{
                    thread->Join();
                    int ex = thread->getExStatus();
                    machine->WriteRegister(2, ex);
                    incrementarPC();
                    break;
                }
            }
            case SC_Exec:{
                DEBUG('e',"EXEC: entra \n");
                int fileAddr = machine->ReadRegister(4);
                int cantArg = machine->ReadRegister(5);
       	        int argsAddr = machine->ReadRegister(6);
       	        
       	        char **args = new char *[cantArg+1];
       	        char* name = new char[128];
       	        
       	        ASSERT(cantArg <= MAX_ARGS);
       	        
       	        ReadStringFromUser(fileAddr, name);
       	        
       	        OpenFile *of = fileSystem->Open(name);
       	        
       	        
       	        if(of == NULL){
       	            DEBUG('e',"Error en Exec: Open File NULL \n");
                    machine -> WriteRegister(2,-1);
                    incrementarPC();
                    break;
       	            
       	        } else {
       	              int i = 0, a;
           	          if (cantArg != 0){
           	              for(i = 0; i < cantArg; i++){
                 	            machine->ReadMem(argsAddr + 4 * i, 4, &a);
                 	            if (a == 0)
                 	              break;   
                 	            char temp[MAX_ARG_LEN];
                 	            ReadStringFromUser(a, temp);
                 	            args[i] = new char [strlen(temp) + 1];
                 	            strcpy(args[i], temp);
             	          }
             	        }
       	        
           	          AddrSpace *s = new AddrSpace(of);
           	          
           	          #ifndef DEMANDA_PURA
           	          delete of;
           	          #endif
       	        
       	        
           	          Thread *t = new Thread(name);
           	          delete[] name;
        
           	          t->space = s;
           	          
       	        
           	          
           	          struct Arg_WriteArgs *arguParaWriteArgs = new struct Arg_WriteArgs;
                      arguParaWriteArgs->c = i;
                      arguParaWriteArgs->v = args;
                      
                      
           	          
           	          t->Fork(startProc, arguParaWriteArgs);
           	          int id = processTable->AddProcess(t);
           	          machine->WriteRegister(2, id);
           	       
           	        incrementarPC();
           	        break;
       	        }
            }
            
        }
    } else if (which == PageFaultException) {
        DEBUG('v',"PageFaultException \n");
        int num_vir_pag = (machine->ReadRegister(BadVAddrReg)) / PageSize;
        int i;

        TranslationEntry* e = currentThread->space->ObtenerPag(num_vir_pag);
        
        for (i = 0; i < TLBSize; i++) {
            if (!machine->tlb[i].valid) {
                break;
            }
        }

        if (i == TLBSize) { //no hay entrada libre
            i = rand() % TLBSize; //seleccionar una entrada para reemplazar
        }

        machine->tlb[i].virtualPage = e->virtualPage;
        machine->tlb[i].physicalPage = e->physicalPage;
        
        if(e->physicalPage == -1){
            ASSERT(false);
        }
        machine->tlb[i].valid = true;
        machine->tlb[i].use = e->use;
        machine->tlb[i].dirty = e->dirty;
        machine->tlb[i].readOnly = e->readOnly; 
        
        } else if (which == ReadOnlyException) {
            	printf("ReadOnlyException\n");
            ASSERT(false);
            } else {
            	printf("Unexpected user mode exception %d %d\n", which, type);
            	ASSERT(false);
             }
}


