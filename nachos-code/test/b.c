#include "syscall.h"

int main()
{
    int i;
    for(i=0;i<100;i++)
        Write("B",1,ConsoleOutput); 
    Exit(0);
}
