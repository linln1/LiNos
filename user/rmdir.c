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
	}
	int ret = delete(argv[1], 2);
	if(ret == 0) {
		printf("dir %s deleted successful!\n", argv[1]);
	}

	return 0;
}
