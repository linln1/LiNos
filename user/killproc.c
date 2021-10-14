#include <stdio.h>
#include <ulib.h>

int
main(void) {
    listProc();
    int pid=fork();
    if(pid==0)
    {
     cprintf("%d",pid);
    }else
    {cprintf("------\n");
    listProc();
    kill(pid);
    cprintf("delete process %d",pid);
    listProc();
   
     }
    return 0;
}


