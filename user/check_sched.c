#include <ulib.h>
#include <stdio.h>

#define printf(...)                     fprintf(1, __VA_ARGS__)


int main(void){
    int i;
    for(i=0;i<5;i++)
    {
        int pid;
        if((pid=fork())==0)
        {
            int p=i;
            yield();
            int j;
            for(j=0;j<=6000000;j++){
                if(j%2000000==0)
                    cprintf("%d \n",p);
            }
            break;
        }
    }
    return 0;
}
