#include "syscall.h"

int
main()
{
    char lee[15];
    OpenFileId o = Open("../test/testleer.txt");  
    Read(&lee,15,o);
    Write(lee,15,1);
    Close(o);
    Halt();
    /* not reached */
}