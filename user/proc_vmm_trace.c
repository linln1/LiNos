#include <stdlib.h>
#include <ulib.h>
#include <string.h>
#include <stdio.h>

// 初始为两个内核线程，本程序为第3号线程，fork()出来的为4号线程，在4号线程中申请内存空间同时在三号线程中观测系统中所有进程内存空间的变化，主要观测四号进程的内存变化 
//此过程在三号进程中输出了了fork()出的四号进程从出生到死的全部内存变化情况
int main(){
        
        listProc();
	int seq_num = rand()%15;
	cprintf("alloc memory for %d times\n", seq_num);
        int mem_seq;
        int j=0;
        int i=0;
	int ret = 0;
       if(fork()==0){
	for(; i < seq_num; i++){
		mem_seq = rand();
		
		while(mem_seq >= 32*1024 || mem_seq <= 4*1024){
			mem_seq = rand();
		}

		cprintf("%d th time, alloc memory about %d\n", i, mem_seq);
                malloc(mem_seq);
		
	}

    }else
        {
            
          
            
           while(j<(seq_num+8))
            {
	   listProc();  
            j++;
            }
	  
        }
	cprintf("ret is %d\n", ret);
	return 0;
}
