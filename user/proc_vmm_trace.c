#include <stdlib.h>
#include <ulib.h>
#include <string.h>
#include <stdio.h>

// ��ʼΪ�����ں��̣߳�������Ϊ��3���̣߳�fork()������Ϊ4���̣߳���4���߳��������ڴ�ռ�ͬʱ�������߳��й۲�ϵͳ�����н����ڴ�ռ�ı仯����Ҫ�۲��ĺŽ��̵��ڴ�仯 
//�˹��������Ž������������fork()�����ĺŽ��̴ӳ���������ȫ���ڴ�仯���
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
