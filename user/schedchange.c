#include <ulib.h>
#include <stdio.h>

#define printf(...)                     fprintf(1, __VA_ARGS__)

int
main(int argc, char **argv){
    if(argc !=  2){
        printf("\nbad argument!\n");
    }
    else{
    	if(argv[1][0]=='1')
    	    change_sched(1);
    	else if(argv[1][0]=='2')	
            change_sched(2);
        else if(argv[1][0]=='3')	
            change_sched(3);
	else if(argv[1][0]=='4')
	    change_sched(4);
        else
            printf("\nbad argument!\n");
    }
    return 0;
}
