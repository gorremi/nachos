#include "syscall.h"

//Consola Concurrente

int main()
{
    int p1,p2;
    p1=Exec("../test/a",0,"");
    p2=Exec("../test/b",0,"");
   // Exit(0);
    Join(p1);
    Join(p2);
    Halt();
}
