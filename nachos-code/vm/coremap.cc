
#include "coremap.h"
#include "machine.h"


CoreMap::CoreMap()
{
    pags = new int[NumPhysPages];
    procs = new AddrSpace*[NumPhysPages];

    for(int i=0;i<NumPhysPages;i++){
        pags[i] = -1;
        procs[i] = NULL;
    }
}

CoreMap::~CoreMap()
{
    delete []pags;
    delete []procs;
}

void CoreMap::Agregar(int pagFis, int pagVir, AddrSpace *pSpace)
{
    pags[pagFis] = pagVir;
    procs[pagFis] = pSpace;
}

void CoreMap::Quitar(int pagFis)
{
    pags[pagFis] = -1;
    procs[pagFis] = NULL;
}

int CoreMap::PaginaVirtual(int pagFis)
{
    return pags[pagFis];
}

AddrSpace* CoreMap::ProcSpace(int pagFis)
{
    return procs[pagFis];
}
