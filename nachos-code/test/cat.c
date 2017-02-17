#include "syscall.h"


int main (int argc, char **argv) 
{ 
   if (argc < 1)  
        Exit(0);
 
   int i;
   char ch[1];
  
   for (i=0; i<argc;i++){
    OpenFileId of = Open(argv[i]);
        if (of != -1) {
            while (Read(ch, 1, of)) 
                Write(ch, 1, ConsoleOutput);
                 
        Close(of);
        }
   }    
   
   Exit(0);
}
