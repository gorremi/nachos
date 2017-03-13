// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "translate.h"


#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 
    
    int Translate_(int virtualAddress);
    
    void WriteArgs(int argc,char **args);
    
    
    TranslationEntry* ObtenerPag(int num_virt_pag);
    
    #ifdef DEMANDA_PURA
    void CargarEnMemoria(int num_virt_pag);
    #endif
    
    #ifdef VM
    void CargarDesdeSwap(int num_virt_pag);
    void EnviarASwap(int num_virt_pag);
    int LiberarPagFisica();
    #endif
    

  private:
	  
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space
  
    //Globales para setearlos en addrspace y poder usarlos en la carga de pag. a memoria despues
    OpenFile *executable;
    
    #ifdef DEMANDA_PURA
    
    int noffH_code_inFileAddr;
    int noffH_code_virtualAddr;
    int noffH_code_size;
    int noffH_initData_inFileAddr;
    int noffH_initData_virtualAddr;
    int noffH_initData_size;
    
    
    // La pag. esta en memoria o hay que ir a buscarla
    bool* enMemoria;
    
    #endif
    
    #ifdef VM
    char* swapFileName;
    OpenFile* swapFile;
    bool* enSwap;
    #endif
    ////////////
    
};

#endif // ADDRSPACE_H
