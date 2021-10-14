#include <ulib.h>
#include <stdio.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)


#define max 5


int 
main(void)
{
    int i,j,pid;
    for(i=0;i<5;i++)
    {
    	if((pid=fork())==0)
    	{
    	    for(j=0;j<5;j++)
    	    {
    	        printf("%d queue:",getpid());
	        getrunqueue();
	        printf("\n");
	        yield();
	    }
	    break;
	}
    	
    }
    return 0;

}


// #include <ulib.h>
// #include <stdio.h>

// #define printf(...)                 fprintf(1, __VA_ARGS__)


// int max=5;

// int 
// main(void)
// {
//     int n=0;
//     child_function(0);

// }

// int child_function(int n)
// {
//     int pid;
//     if(n<max)
//     {
//         if((pid=fork())==0)
//         {
            
//             child_function(n+1);
//         }
//         while(1)
//         {
//             printf("\n%d queue:",getpid());
//             getrunqueue();
            
//         }
    
//     }
    
//     // int pid
//     // while((pid=getrunqueue()!=NULL)){
//     //     printf("\t%d",pid)        
//     // }


// }