#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>
#include <stat.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)




int main(int argc, char** argv)
{
	if(argc != 3){
		printf("[error] : wrong number of params! \n");	
		return -1;
	}

	int ret = rename(argv[1], argv[2]);

	if(ret == 0)
		printf("rename successful!\n");

	return 0;
}
