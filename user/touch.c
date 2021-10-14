#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)

static int safe_open(const char *path, int open_flags)
{
	int fd = open(path, open_flags);
	printf("fd is %d\n",fd);
	assert(fd >= 0);
	return fd;
}

static struct stat *safe_fstat(int fd)
{
	static struct stat __stat, *stat = &__stat;
	int ret = fstat(fd, stat);
	assert(ret == 0);
	return stat;
}


static void safe_read(int fd, void *data, size_t len)
{
	int ret = read(fd, data, len);
	assert(ret == len);
}


int main(int argc, char** argv)
{
	if(argc != 2){
		printf("[error]: wrong number of params\n");
	}

	char *file_name = argv[1];
        char path[256];
        int i;
        for(i = 0 ; i < 256 && file_name[i] != '\0'; i++){
        	path[i] = file_name[i];
        }
	path[i] = '\0';
	
	int fd1 = safe_open(path, O_CREAT);
	struct stat *stat = safe_fstat(fd1);
	assert(stat->st_size >= 0 && stat->st_blocks >= 0);
	printf("%s created successful!\n", path);
	printf("fd1 = %d\n", fd1);

	int ret = close(fd1);
	printf("close'ret = %d\n", ret);

	int fd2 = safe_open(path, O_RDONLY);
	struct stat *stat2 = safe_fstat(fd2);
	assert(stat2->st_size >= 0 && stat2->st_blocks >= 0);
	printf("%s created successful!\n", path);
	printf("fd2 = %d\n", fd2);

	close(fd2);

	return 0;
}
