#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)

int main(int argc, char** argv)
{	
	
	if(argc != 2){
		printf("[error] : wrong number of params! \n");	
		return -1;
	}
	char *dir_name = argv[1];
        char path[256];
        int i;
        for(i = 0 ; i < 256 && dir_name[i] != '\0'; i++){
        	path[i] = dir_name[i];
        }
	path[i] = '\0';
	int ret = mkdir(path);
	if(ret == 0) {
		printf("dir %s created successful!\n", path);
	}
	return 0;
}
