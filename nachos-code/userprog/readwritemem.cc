
/*
Ejercicio 1 plancha 3
  void ReadStringFromUser(int userAddress, char *outString);
  void ReadBufferFromUser(int userAddress, char *outBuffer, int byteCount);
  void WriteStringToUser(char *string, int userAddress);
  void WriteBufferToUser(char *string, int userAddress, int byteCount);
*/

#include "readwritemem.h"

void ReadStringFromUser(int userAddress, char *outString){
    int aux;
    int i=0;
    while(true){
        DEBUG('s',"en while de ReadStringFromUser \n");
        if(!machine->ReadMem(userAddress+i,1,&aux))
            ASSERT(machine->ReadMem(userAddress+i,1,&aux));
        outString[i] = aux;
        if(aux==0) 
            return;
        i++;
    }
}

void ReadBufferFromUser(int userAddress, char *outBuffer, int byteCount){
    int aux;
    int i;
    for(i=0 ; i < byteCount ; i++){
        if(!machine->ReadMem(userAddress+i,1,&aux)) 
            ASSERT(machine->ReadMem(userAddress+i,1,&aux));
        outBuffer[i] = aux;
    }
}


void WriteStringToUser(char *string, int userAddress){
    int i=0;
    while(true){
        if(!machine->WriteMem(userAddress+i,1,string[i]))
            ASSERT(machine->WriteMem(userAddress+i,1,string[i]));
        if (string[i]==0)
            return;
        i++;    
    }
}


void WriteBufferToUser(char *string, int userAddress, int byteCount){
    int i;
    for(i=0 ; i < byteCount ; i++){
        if(!machine->WriteMem(userAddress+i,1,string[i]))
            ASSERT(machine->WriteMem(userAddress+i,1,string[i]));
    }
}