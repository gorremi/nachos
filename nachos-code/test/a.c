#include "syscall.h"

int main()
{
    int i;
    for(i=0;i<100;i++)
        Write("A",1,ConsoleOutput); 
    Exit(0);
}
