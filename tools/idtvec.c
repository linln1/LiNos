#include <libs/stdio.h>

int main(){
    printf("# handler\n.txt\n.globl __alltraps\n");

    for(int i = 0 ; i < 256 ; i++){
        printf(".globl vector%d\n", i);
        printf("vector%d:\n", i);
        if((i<8 || i>14) && i!=17){
            printf(" pushl $0\n");
        }
        printf("  push $%d\n", i);
        printf("  jmp __alltraps\n");
    }
    printf("\n");
    printf("# vector table\n");
    printf(".data\n");
    printf(".globl __vectors\n");
    printf("__vectors:\n");
    for(int i = 0 ; i < 256 ; i++){
        printf("  .long vector%d\n", i);
    }
    return 0;
}