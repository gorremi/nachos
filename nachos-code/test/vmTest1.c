#include "syscall.h"


int main (int argc, char **argv) 
{ 
    SpaceId proc1,proc2,proc3;
  
    proc1 = Exec("../test/holamundo",0,""); 
    proc2 = Exec("../test/matmult",0,"");
    proc3 = Exec("../test/sort",0,"");
    Join(proc3);

   Exit(0);
}
