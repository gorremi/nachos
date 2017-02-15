#include "processtable.h"


ProcessTable::ProcessTable() {
    t_procesos = new Thread*[MAX_PROC];
    int id;

    for (id = 0; id < MAX_PROC; id++) {
        t_procesos[id] = NULL;
    }
}


ProcessTable::~ProcessTable() {
    delete[] t_procesos;
}


int ProcessTable::AddProcess(Thread* thread) {
    int id;
    
    for (id = 0; id < MAX_PROC; id++) {
        if (t_procesos[id] == NULL) {
            t_procesos[id] = thread;
            return id;
        }
    }
    return -1; // No hay id libre
}


Thread* ProcessTable::GetProcess(int id) {
    if (id >= 0 && id < MAX_PROC) {
        return t_procesos[id];
    } else {
        return NULL;
    }
}


int ProcessTable::GetID(Thread* thread) {
    int id;
    for (id = 0; id < MAX_PROC; id++) {
        if (t_procesos[id] == thread) {
            return id;
        }
    }
    return -1; // el proceso no esta en la tabla
}


void ProcessTable::RemoveProcess(int id) {
    t_procesos[id] = NULL;
}
