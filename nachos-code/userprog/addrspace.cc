// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

#include <string.h>
#include "readwritemem.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *execArg)
{
    NoffHeader noffH;
    executable = execArg;
    
    unsigned int i, size;


    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    #ifndef VM
    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory
	#else //VM
	
	enSwap = new bool[numPages];
	
    #endif

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    
    #ifdef DEMANDA_PURA
    enMemoria = new bool[numPages];
    #endif
    
    int plibre;
    char *page;

    for (i = 0; i < numPages; i++) {
    	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
    	//pageTable[i].physicalPage = i;
    	#ifdef DEMANDA_PURA // SI DEMANDA PURA
    	pageTable[i].physicalPage = -1;
    	pageTable[i].valid = false;
    	enMemoria[i]=false;
    	#else // SI NO DEMANDA PURA
    	plibre = pagLibres->Find();
    	ASSERT(plibre != -1);
    	pageTable[i].physicalPage = plibre;
    	pageTable[i].valid = true;
    	#endif //FIN DE SI NO DEMANDA PURA
    	#ifdef VM
    	enSwap[i]=false;
    	#endif
    	
    	pageTable[i].use = false;
    	pageTable[i].dirty = false;
    	pageTable[i].readOnly = false;  // if the code segment was entirely on 
    					// a separate page, we could set its 
    					// pages to be read-only
        
        #ifndef DEMANDA_PURA // SI NO DEMANDA PURA
        page = &machine->mainMemory[plibre * PageSize];
    	bzero(page, PageSize);    
        #endif // FIN DE SI NO DEMANDA PURA 
    }
    

    #ifndef DEMANDA_PURA // SI NO DEMANDA PURA

    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
        
        for(int j=0; j<noffH.code.size;j++){
            char c;
            executable->ReadAt(&c, 1, noffH.code.inFileAddr + j);
            machine->mainMemory[Translate_(noffH.code.virtualAddr + j)] = c;
        }
    }
    
    
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
		
		for(int j=0;j<noffH.initData.size;j++){
		    char c;
		    executable->ReadAt(&c, 1, noffH.initData.inFileAddr + j);
		    machine->mainMemory[Translate_(noffH.initData.virtualAddr + j)] = c;
		}	
    }
    
    #else // SI DEMANDA PURA
    noffH_code_inFileAddr = noffH.code.inFileAddr;
    noffH_code_virtualAddr = noffH.code.virtualAddr;
    noffH_code_size = noffH.code.size;
    noffH_initData_inFileAddr = noffH.initData.inFileAddr;
    noffH_initData_virtualAddr = noffH.initData.virtualAddr;
    noffH_initData_size = noffH.initData.size;
    #endif // FIN DE SI DEMANDA PURA
    
    #ifdef VM
    
    int id = processTable->GetID(currentThread);
    swapFileName = new char[128];
    snprintf(swapFileName, 128, "SWAP.%p", this);
    fileSystem->Create(swapFileName, size);
    swapFile = fileSystem->Open(swapFileName);
    
    #endif
    
    
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    int i;
  
    for (i = 0;i < (int) numPages; i++)
        if(pageTable[i].valid){
            pagLibres->Clear(pageTable[i].physicalPage);
            #ifdef VM
            coremap->Quitar(pageTable[i].physicalPage);
            #endif
        }

    delete[] pageTable;

    
    #ifdef VM
    fileSystem->Remove(swapFileName);
    delete[] swapFileName;
    delete swapFile; 
    delete[] enSwap;
    #endif            
    
    #ifdef DEMANDA_PURA
    delete[] enMemoria;
    #endif
   
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{
#ifdef USE_TLB
    int i;
    for (i = 0; i < TLBSize; i++) {
        TranslationEntry e= machine->tlb[i];
        if (e.valid && e.dirty) {
            pageTable[e.virtualPage] = e;
            machine->tlb[i].valid=false;
        }
    }
#endif

}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
#ifdef USE_TLB
    int i;
    for (i = 0; i<TLBSize;i++)
        machine->tlb[i].valid=false;
        
#else // FIN USE_TLB
        machine->pageTable = pageTable;
        machine->pageTableSize = numPages;
#endif
}

int AddrSpace::Translate_(int virtualAddress) {
    int virtualPage = virtualAddress / PageSize;
    int offset = virtualAddress % PageSize;
    return pageTable[virtualPage].physicalPage * PageSize + offset;
}


// Para argumentos en Exec

void AddrSpace::WriteArgs(int argc,char **args){
    
    DEBUG('e', "Writing command line arguments into child process.\n");
    
    int *addr = new int[argc];
    int i;
    int sp = machine->ReadRegister(StackReg);
    
   
        
    for (i = 0; i < argc; i++){
        sp -= strlen(args[i]) + 1;
        WriteStringToUser(args[i], sp);
        addr[i] = sp;    
    }
    

    // Alineamos la pila a múltiplo de cuatro.  
    sp -= sp % 4;
    // Hacemos lugar para el arreglo y el último NULL.
   // sp -= 4 * argc + 4;
    sp -= 4 * argc;
  
    
    for (i = 0; i < argc; i++){
        machine->WriteMem(sp, 4, addr[i]);
        sp += 4;
    }
    
    //machine->WriteMem(sp, 4, 0);
    
    sp -= (4 * argc);
    //sp -= 16;
    machine->WriteRegister(StackReg, sp-16); 
    machine->WriteRegister(4, argc); 
    machine->WriteRegister(5, sp);
      
    delete[] addr;
    
    for (i = 0; i < argc; i++){
        delete[] args[i];
    }
    delete[] args;

}

// Para resolver PageFaultException

TranslationEntry* AddrSpace::ObtenerPag(int num_virt_pag)
{
    DEBUG('v', "Necesita la PAG virtual %d \n",num_virt_pag);
    
    #ifdef DEMANDA_PURA
    if(!enMemoria[num_virt_pag]){
        #ifdef VM
            if(!enSwap[num_virt_pag])
                CargarEnMemoria(num_virt_pag);
            else
                CargarDesdeSwap(num_virt_pag);
        #else
        CargarEnMemoria(num_virt_pag);
        #endif
        
    }
    #endif    
    
    ASSERT(pageTable[num_virt_pag].virtualPage == num_virt_pag  );
    return &pageTable[num_virt_pag]; 
    
}


#ifdef DEMANDA_PURA // SI DEMANDA PURA

void AddrSpace::CargarEnMemoria(int num_virt_pag) {
    
    int virt_Addr;
    virt_Addr = num_virt_pag * PageSize;
    
    int plibre;
    
    plibre = pagLibres->Find();
    
    if (plibre == -1){
        #ifdef VM
        plibre = LiberarPagFisica();
        pagLibres->Mark(plibre);
        #else
        ASSERT(false);
        #endif
    } 
    
    pageTable[num_virt_pag].physicalPage = plibre;  
    
    #ifdef VM
    coremap->Agregar(pageTable[num_virt_pag].physicalPage,num_virt_pag,this);
    pagEnMem->Append(pageTable[num_virt_pag].physicalPage);
    #endif
   
    DEBUG('v', "Carga la pag. virtual %d en memoria \n", num_virt_pag);

    if (noffH_code_size > 0) {
        executable->ReadAt(&(machine->mainMemory[Translate_(virt_Addr)]), PageSize,noffH_code_inFileAddr + virt_Addr - noffH_code_virtualAddr);
    } 

    if (noffH_initData_size > 0) {
        executable->ReadAt(&(machine->mainMemory[Translate_(virt_Addr)]), PageSize, noffH_initData_inFileAddr + virt_Addr - noffH_initData_virtualAddr);
    }
    
    DEBUG('v', "Fin de Carga la pag. en memoria \n");
    
    pageTable[num_virt_pag].valid = true;
    enMemoria[num_virt_pag] = true;
    pageTable[num_virt_pag].virtualPage = num_virt_pag; 
    
}

#endif

#ifdef VM

/*

REMPLAZO DE PAG. FISICA CON FIFO:

int AddrSpace::LiberarPagFisica(){
    DEBUG('v', "Tiene que liberar una pag de memoria \n");
    int pagAeliminar,pagVirtual;
    AddrSpace* procSpace;
    
    pagAeliminar = pagEnMem->Remove();

    pagVirtual = coremap->PaginaVirtual(pagAeliminar);
    procSpace = coremap->ProcSpace(pagAeliminar);

    procSpace->EnviarASwap(pagVirtual);
    bzero(machine->mainMemory+pagAeliminar*PageSize,PageSize);
    
    DEBUG('v', "libera PAG FISICA %d \n",pagAeliminar);
    return pagAeliminar;
}

*/ 

// REMPLAZO DE PG. FISICA CON ALGORITMO DE SGUNDA OPORTUNIDAD MEJORADA:

int AddrSpace::LiberarPagFisica(){
    DEBUG('v', "Tiene que liberar una pag de memoria \n");
    int pagAeliminar,pagVirtual;
    AddrSpace* procSpace;
    
    int vuelta ;
    for (vuelta = 1; vuelta <=4; vuelta++ ){
        for (int i =0; i<NumPhysPages; i++){
            pagAeliminar = pagEnMem->Remove();
            
            bool use = pageTable[coremap->PaginaVirtual(pagAeliminar)].use;
            bool dirty = pageTable[coremap->PaginaVirtual(pagAeliminar)].dirty;
            if ((vuelta == 1) && !use && !dirty )
                break;
            if ((vuelta == 2) && !use && dirty )
                break;
            if ((vuelta == 3) && use && !dirty )
                break;    
            if ((vuelta == 4) && use && dirty )
                break;  
            
            pagEnMem->Prepend(pagAeliminar);
        }
            
    }

    pagVirtual = coremap->PaginaVirtual(pagAeliminar);
    procSpace = coremap->ProcSpace(pagAeliminar);

    procSpace->EnviarASwap(pagVirtual);
    bzero(machine->mainMemory+pagAeliminar*PageSize,PageSize);
    
    DEBUG('v', "libera PAG FISICA %d \n",pagAeliminar);
    return pagAeliminar;
}








void AddrSpace::EnviarASwap(int num_virt_pag){
    
    DEBUG('v', "Envía a PAG VIRTUAL %d a Swap \n", num_virt_pag);
    
    int pagFisica, dirFisica, dirVirtual;
    pagFisica = pageTable[num_virt_pag].physicalPage;
    dirFisica= pagFisica * PageSize;
    dirVirtual = num_virt_pag * PageSize;
    
    swapFile->WriteAt(&(machine->mainMemory[dirFisica]), PageSize, dirVirtual);
    
    pagLibres->Clear(pagFisica);
    coremap->Quitar(pagFisica);
    
    pageTable[num_virt_pag].valid=false;
    pageTable[num_virt_pag].physicalPage=-1;
    
    
    for(int i=0;i<TLBSize;i++)
        if(machine->tlb[i].physicalPage == pagFisica){
            machine->tlb[i].valid=false;
            break;
        }
            
    enSwap[num_virt_pag]=true;
    enMemoria[num_virt_pag]=false;
    
}


void AddrSpace::CargarDesdeSwap(int num_virt_pag){
    
    DEBUG('v', "Trae la PAG VIRTUAL %d desde Swap \n",num_virt_pag);
    
    int pagFisica, dirFisica, dirVirtual;
    
    dirVirtual = num_virt_pag * PageSize;
    
    int plibre;
    plibre = pagLibres->Find();
    
    if (plibre == -1)
        plibre = LiberarPagFisica();
    
    
    pagFisica = plibre;
    dirFisica = pagFisica * PageSize;
    
    pageTable[num_virt_pag].physicalPage = pagFisica;
    ASSERT(pageTable[num_virt_pag].physicalPage < 32);
    
    pagLibres->Mark(plibre);
    
    coremap->Agregar(pagFisica, num_virt_pag, this);
    
    swapFile->ReadAt(&(machine->mainMemory[dirFisica]), PageSize, dirVirtual);

    
    pageTable[num_virt_pag].valid=true;
    pagEnMem->Append(pagFisica);
    
    enMemoria[num_virt_pag]=true;
    enSwap[num_virt_pag]=false;
}

#endif



