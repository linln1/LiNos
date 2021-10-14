#include <stdio.h>
#include <ulib.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)

#define MAX_TICKS 100000000

int 
main(void)
{
    int i,j,pid;
    lab6_set_priority(60000000);
    for(i=0;i<5;i++)
    {
    	
    	if((pid=fork())==0)
    	{
    	    lab6_set_priority( (i+1)*10000000 );
	    int i=0;
	    yield();
    	    while(i<MAX_TICKS){
		i++;
	    }
	    break;
	}
    	
    }
    return 0;

}
