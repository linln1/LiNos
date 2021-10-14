#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)

static int safe_delete(const char *path, int type) {
	int ret = delete(path, type);
	assert(ret == 0);
	printf("file named '%s' deleted\n", path);
	return ret;
}

int main(int argc, char** argv)
{
	if(argc != 2){
		printf("[error] : wrong number of params\n");	
	}
	safe_delete(argv[1], 1);
	return 0;
}
