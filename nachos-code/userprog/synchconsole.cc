#include "synchconsole.h"


//----------------------------------------------------------------------
// En synchdisk.h para DiskRequestDone dice:
// 	"Disk interrupt handler.  Need this to be a C routine, because 
//	C++ can't handle pointers to member functions."
//  Hago idem para synchconsole
//----------------------------------------------------------------------

static void ReadRequestDone(void* arg) {
    SynchConsole* synchConsole = (SynchConsole*) arg;
    synchConsole->ReadAvailable();
}

static void WriteRequestDone(void* arg) {
    SynchConsole* synchConsole = (SynchConsole*) arg;
    synchConsole->WriteDone();
}

// 


SynchConsole::SynchConsole(const char* readFile, const char* writeFile) {
    console = new Console(readFile, writeFile, ReadRequestDone, WriteRequestDone, this);
    readSemaphore = new Semaphore("Sem de lectura consola", 0);
    writeSemaphore = new Semaphore("Sem de escritura consola", 0);
    readLock = new Lock("Lock de lectura consola");
    writeLock = new Lock("Lock de escritura consola");

}

SynchConsole::~SynchConsole() {
    delete console;
    delete readSemaphore;
    delete writeSemaphore;
    delete readLock;
    delete writeLock;
}

char SynchConsole::GetChar() {
    readLock->Acquire();
    readSemaphore->P();
    char c = console->GetChar();
    readLock->Release();
    return c;
}

void SynchConsole::PutChar(const char c) {
    writeLock->Acquire();
    console->PutChar(c);
    writeSemaphore->P();
    writeLock->Release();
}

void SynchConsole::ReadAvailable() {
    readSemaphore->V();
}

void SynchConsole::WriteDone() {
    writeSemaphore->V();
}