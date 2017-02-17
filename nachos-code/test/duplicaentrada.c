#include "syscall.h"


// Ejemplo para exec con argumento

int strlen(char *s){
    int i;
    for(i=0;s[i];i++) ;
    return i;
}



int main (int argc, char **argv) 
{ 
   if (argc < 1)  
    Exit(0);
 
   int i;
  
   for (i=0; i<argc;i++){
        Write(argv[i],strlen(argv[i]), ConsoleOutput);
        Write("//",2, ConsoleOutput);
        Write(argv[i],strlen(argv[i]), ConsoleOutput);
        Write("\n",1, ConsoleOutput);
     }
  
   Exit(0);
}