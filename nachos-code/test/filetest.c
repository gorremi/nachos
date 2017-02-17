#include "syscall.h"

int
main()
{
    Create("../test/test.txt");
    OpenFileId o = Open("../test/test.txt");  
    Write("TEST CREAR ARCHIVO Y ESCRIBIR EN EL\n",35,o);
    Close(o);
    Halt();
    /* not reached */
}