#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>
#include <dir.h>


#define printf(...)                 fprintf(1, __VA_ARGS__)

int main(){
	char buffer[64];
	getcwd(&buffer, 30);
	printf("current path = %s\n", buffer);
	return 0;
}
