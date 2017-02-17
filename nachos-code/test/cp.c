#include "syscall.h"


int main (int argc, char **argv) 
{ 
    if (argc < 2)  
        Exit(0);
 

    OpenFileId origen = Open(argv[0]);
    if (origen == -1) {
            Write("No existe el archivo origen",27,ConsoleOutput);
            Exit(-1);
        }
        
    
    Create(argv[1]);
    OpenFileId destino = Open(argv[1]);

     char ch[1];
      
    while(Read(ch,1,origen)){
        Write(ch,1,destino);
    }
    
    
    Close(origen);
    Close(destino);    
  
   
   Exit(0);
}
