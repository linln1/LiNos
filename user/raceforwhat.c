#include <stdio.h>
#include <ulib.h>

int
main(void) {
    listwait();
    if(fork()==0)
    {
     cprintf("");
    }else
    {
    listwait();
     }
    return 0;
}
