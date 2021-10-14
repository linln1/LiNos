#include <stdio.h>
#include <ulib.h>

int
main(void) {
    listProc();
    if(fork()==0)
    {
     cprintf(" ");
    }else
    {cprintf("------\n");
    listProc();
    
    listProc();
   
     }
    return 0;
}

