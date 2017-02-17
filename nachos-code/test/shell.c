#include "syscall.h"


int strlen(char *s){
    int i;
    for(i=0;s[i];i++) ;
    return i;
}

void strcpy(char *origen, char *destino){
    int i;
    for(i=0;origen[i]!='\0';i++)
        destino[i] = origen[i];
    destino[i] = '\0';
}

int
main()
{
    SpaceId newProc;
    SpaceId proc2;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
   // char prompt[2], ch, buffer[60], file[60];
   // int i,j,hayArgumentos;
   // char argumentos[MAX_ARGS][60];

    char prompt[2];
    
    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
    char ch, buffer[60], file[60];
    int i,j,hayArgumentos;
    char argumentos[MAX_ARGS][60];	
    
    	
	Write(prompt, 2, output);
	
	
	
	int cantidadArg=0;
    i = 0;
    
	while(1){
	    Read(&buffer[i], 1, input);
	    if (buffer[i]== '\n'){
	        hayArgumentos=0;
	        break;
	    }
	    if (buffer[i]==' '){
	        hayArgumentos=1;
	        cantidadArg++;
	        break;
	    }
	    file[i]=buffer[i];
	    i++;
	}
	file[i] = '\0';
	
	j=0;
	while(hayArgumentos){
	    char buffer[60];
	    int y;
	    y=0;
	    while(1){
	        Read(&buffer[y], 1, input);
	        if (buffer[y]== '\n'){
	            hayArgumentos=0;
	            break;
	        }
	        if (buffer[y]==' '){
	            hayArgumentos=1;
	            cantidadArg++;
	            break;
	        }
	        y++;
	    }
	        buffer[y] = '\0';
	        strcpy(buffer,argumentos[j]);
	        //argumentos[j]=buffer;
	        
	        j++;
	  }
	
    int k;
    char* arguExec[MAX_ARGS];
    for( k=0; k<cantidadArg; k++){
       arguExec[k]=argumentos[k];
    }
   
    
	if( i > 0 ) {
	   if(file[0]=='&'){
			newProc = Exec(file+1,cantidadArg,arguExec);
	   } else{
	   		newProc = Exec(file,cantidadArg,arguExec);
	   		Join(newProc);
	   }
	   
	}
  }
}

