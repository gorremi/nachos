
#include "addrspace.h"

class CoreMap {
  public:
    CoreMap();
    ~CoreMap();

    void Agregar(int paginaFisica, int paginaVirtual, AddrSpace *procSpace);
    void Quitar(int paginaFisica);

    int PaginaVirtual(int paginaFisica);
    AddrSpace *ProcSpace(int paginaFisica);

  private:
    int *pags;
    AddrSpace **procs;
};