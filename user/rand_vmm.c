#include <stdlib.h>
#include <ulib.h>
#include <string.h>
#include <stdio.h>


int main(){
        //current_vm();

	int seq_num = rand()%10;
	cprintf("alloc memory for %d times\n", seq_num);

	int mem_seq;
	int i = 0;
	int ret = 0;
	for(; i < seq_num; i++){
		mem_seq = rand();
		
		while(mem_seq >= 32*1024 || mem_seq <= 4*1024){
			mem_seq = rand();
		}

		cprintf("%d th time, alloc memory about %d\n", i, mem_seq);
		ret = malloc(mem_seq);
	}
	cprintf("ret is %d \n", ret);
	return 0;
}
