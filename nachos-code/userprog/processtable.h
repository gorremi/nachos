#include "thread.h"

#define MAX_PROC 1024

class ProcessTable {
 public:
    ProcessTable();
    ~ProcessTable();
    
    int AddProcess(Thread* thread);
    Thread* GetProcess(int id);
    int GetID(Thread* thread);
    void RemoveProcess(int id);
    
 private:
    Thread** t_procesos;
};
